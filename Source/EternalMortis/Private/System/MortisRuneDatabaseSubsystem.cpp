// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MortisRuneDatabaseSubsystem.h"
#include "System/MortisGameDataSettings.h"

#include "MortisDebugHelper.h"


void UMortisRuneDatabaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

    const UMortisGameDataSettings* Settings = GetDefault<UMortisGameDataSettings>();
    check(Settings);

    RuneSymbolTable = Settings->RuneSymbolTable.LoadSynchronous();
    RuneSetTable = Settings->RuneSetTable.LoadSynchronous();
    DropRuleTable = Settings->DropRuleTable.LoadSynchronous();
    RuneGradeStyleTable = Settings->RuneGradeStyleTable.LoadSynchronous();

    checkf(RuneSymbolTable, TEXT("RuneSymbolTable is not set in MortisGameDataSettings."));
    checkf(RuneSetTable, TEXT("RuneSetTable is not set in MortisGameDataSettings."));
    checkf(DropRuleTable, TEXT("DropRuleTable is not set in MortisGameDataSettings."));
    checkf(RuneGradeStyleTable, TEXT("RuneGradeStyleTable is not set in MortisGameDataSettings."));

	BuildCaches();
}

void UMortisRuneDatabaseSubsystem::BuildCaches()
{
    RuneSymbolMap.Empty();
    RuneSetMap.Empty();
    DropRulesByFloor.Empty();
    ValidRuneSetTags.Empty();

    if (RuneSymbolTable)
    {
        TArray<FMortisRuneSymbolRow*> Rows;
        RuneSymbolTable->GetAllRows(TEXT("RuneSymbolTable"), Rows);

        for (FMortisRuneSymbolRow* Row : Rows)
        {
            if (!Row) continue;
            RuneSymbolMap.Add(Row->SymbolType, *Row);
        }
    }

    if (RuneSetTable)
    {
        TArray<FMortisRuneSetRow*> Rows;
        RuneSetTable->GetAllRows(TEXT("RuneSetTable"), Rows);
        for (FMortisRuneSetRow* Row : Rows)
        {
            if (!Row) continue;
            RuneSetMap.Add(Row->SetTag, *Row);

            if (Row->SetTag.IsValid() && Row->AllowedSymbols.Num() > 0)
                ValidRuneSetTags.Add(Row->SetTag);
        }
    }

    if (DropRuleTable)
    {
        TArray<FMortisDropRuleRow*> Rows;
        DropRuleTable->GetAllRows(TEXT("DropRuleTable"), Rows);

        for (FMortisDropRuleRow* Row : Rows)
        {
            if (!Row || !Row->IsValid()) continue;
            DropRulesByFloor.Add(Row->Floor, *Row);
        }
    }
}

FMortisRuneInstance UMortisRuneDatabaseSubsystem::GenerateRune(int32 CurrentFloor) const
{
    FMortisRuneInstance NewRune;
    NewRune.InstanceId = FGuid::NewGuid();

    const FMortisDropRuleRow* Rule = GetDropRuleForFloor(CurrentFloor);
    if (!Rule) return NewRune;

    // 등급 선택
    EMortisRuneGrade PickedGrade;
    if (!PickRandomGrade(Rule->RuneGradeWeights, PickedGrade)) return NewRune;

    // 세트 선택
    if (ValidRuneSetTags.Num() <= 0) return NewRune;
    const int32 SetIndex = FMath::RandRange(0, ValidRuneSetTags.Num() - 1);
    const FGameplayTag PickedSetTag = ValidRuneSetTags[SetIndex];

    // 세트 조회
    const FMortisRuneSetRow* SetRow = GetRuneSetRow(PickedSetTag);
    if (!SetRow || SetRow->AllowedSymbols.Num() <= 0) return NewRune;

    // 문양 선택
    const int32 SymbolIndex = FMath::RandRange(0, SetRow->AllowedSymbols.Num() - 1);
    const EMortisRuneSymbol PickedSymbol = SetRow->AllowedSymbols[SymbolIndex];
    if (PickedSymbol == EMortisRuneSymbol::None) return NewRune;

    // 값 랜덤
    const FMortisRuneSymbolRow* SymbolRow = GetRuneSymbolRow(PickedSymbol);
    if (!SymbolRow) return NewRune;
    const FMortisRuneValueRange Range = GetValueRangeByGrade(*SymbolRow, PickedGrade);

    // 룬 반영
    NewRune.SetTag = PickedSetTag;
    NewRune.SymbolType = PickedSymbol;
    NewRune.Grade = PickedGrade;
    NewRune.RolledValue = FMath::FloorToFloat(FMath::FRandRange(Range.MinValue, Range.MaxValue));
    NewRune.GradeStyleRow.DataTable = RuneGradeStyleTable;
    NewRune.GradeStyleRow.RowName = FName(*StaticEnum<EMortisRuneGrade>()->GetNameStringByValue((int64)NewRune.Grade));

    return NewRune;
}

FMortisRuneInstance UMortisRuneDatabaseSubsystem::GenerateRuneWithTag(FGameplayTag SetTag, EMortisRuneGrade Grade) const
{
    FMortisRuneInstance NewRune;

    const FMortisRuneSetRow* SetRow = GetRuneSetRow(SetTag);
    if (!SetRow || SetRow->AllowedSymbols.Num() <= 0)
        return NewRune;

    const int32 SymbolIndex = FMath::RandRange(0, SetRow->AllowedSymbols.Num() - 1);
    const EMortisRuneSymbol PickedSymbol = SetRow->AllowedSymbols[SymbolIndex];
    if (PickedSymbol == EMortisRuneSymbol::None)
        return NewRune;

    const FMortisRuneSymbolRow* SymbolRow = GetRuneSymbolRow(PickedSymbol);
    if (!SymbolRow)
        return NewRune;

    const FMortisRuneValueRange Range = GetValueRangeByGrade(*SymbolRow, Grade);

    NewRune.InstanceId = FGuid::NewGuid();
    NewRune.SetTag = SetTag;
    NewRune.SymbolType = PickedSymbol;
    NewRune.Grade = Grade;
    NewRune.RolledValue = FMath::FloorToFloat(FMath::FRandRange(Range.MinValue, Range.MaxValue));
    NewRune.GradeStyleRow.DataTable = RuneGradeStyleTable;
    NewRune.GradeStyleRow.RowName = FName(*StaticEnum<EMortisRuneGrade>()->GetNameStringByValue((int64)NewRune.Grade));

    return NewRune;
}

FMortisRuneInstance UMortisRuneDatabaseSubsystem::GenerateRandomRuneWithTag(FGameplayTag SetTag, int32 CurrentFloor) const
{
    FMortisRuneInstance NewRune;

    const FMortisRuneSetRow* SetRow = GetRuneSetRow(SetTag);
    if (!SetRow || SetRow->AllowedSymbols.Num() <= 0)
        return NewRune;

    const FMortisDropRuleRow* Rule = GetDropRuleForFloor(CurrentFloor);
    if (!Rule)
        return NewRune;

    EMortisRuneGrade PickedGrade;
    if (!PickRandomGrade(Rule->RuneGradeWeights, PickedGrade))
        return NewRune;

    return GenerateRuneWithTag(SetTag, PickedGrade);
}

FMortisRuneInstance UMortisRuneDatabaseSubsystem::GenerateRuneWithTagAndSymbol(FGameplayTag SetTag, EMortisRuneGrade Grade, EMortisRuneSymbol SymbolType) const
{
    FMortisRuneInstance NewRune;

    if (SymbolType == EMortisRuneSymbol::None)
        return NewRune;

    const FMortisRuneSetRow* SetRow = GetRuneSetRow(SetTag);
    if (!SetRow)
        return NewRune;

    if (!SetRow->AllowedSymbols.Contains(SymbolType))
        return NewRune;

    const FMortisRuneSymbolRow* SymbolRow = GetRuneSymbolRow(SymbolType);
    if (!SymbolRow)
        return NewRune;

    const FMortisRuneValueRange Range = GetValueRangeByGrade(*SymbolRow, Grade);

    NewRune.InstanceId = FGuid::NewGuid();
    NewRune.SetTag = SetTag;
    NewRune.SymbolType = SymbolType;
    NewRune.Grade = Grade;
    NewRune.RolledValue = FMath::FloorToFloat(FMath::FRandRange(Range.MinValue, Range.MaxValue));
    NewRune.GradeStyleRow.DataTable = RuneGradeStyleTable;
    NewRune.GradeStyleRow.RowName = FName(*StaticEnum<EMortisRuneGrade>()->GetNameStringByValue((int64)NewRune.Grade));

    return NewRune;
}

bool UMortisRuneDatabaseSubsystem::GetRuneGradeStyleByHandle(const FDataTableRowHandle& InHandle, FMortisRuneGradeStyleRow& OutStyleRow) const
{
    if (const FMortisRuneGradeStyleRow* FoundRow = InHandle.GetRow<FMortisRuneGradeStyleRow>(TEXT("GetRuneGradeStyleByHandle")))
    {
        OutStyleRow = *FoundRow;
        return true;
    }

    return false;
}

const FMortisRuneSymbolRow* UMortisRuneDatabaseSubsystem::GetRuneSymbolRow(EMortisRuneSymbol SymbolType) const
{
	return RuneSymbolMap.Find(SymbolType);
}

const FMortisRuneSetRow* UMortisRuneDatabaseSubsystem::GetRuneSetRow(FGameplayTag SetTag) const
{
	return RuneSetMap.Find(SetTag);
}

FText UMortisRuneDatabaseSubsystem::GetRuneSetDisplayName(FGameplayTag SetTag) const
{
    if (!SetTag.IsValid())
    {
        return FText::GetEmpty();
    }

    if (const FMortisRuneSetRow* RuneSetRow = GetRuneSetRow(SetTag))
    {
        if (!RuneSetRow->SetName.IsEmpty())
        {
            return RuneSetRow->SetName;
        }
    }

    return FText::FromName(SetTag.GetTagName());
}

const FMortisDropRuleRow* UMortisRuneDatabaseSubsystem::GetDropRuleForFloor(int32 Floor) const
{
    if (Floor <= 0 || DropRulesByFloor.Num() == 0)
        return nullptr;

    return DropRulesByFloor.Find(Floor);
}

FMortisRuneValueRange UMortisRuneDatabaseSubsystem::GetValueRangeByGrade(const FMortisRuneSymbolRow& SymbolRow, EMortisRuneGrade Grade)
{
    switch (Grade)
    {
    case EMortisRuneGrade::Common:    return SymbolRow.CommonRange;
    case EMortisRuneGrade::Rare:      return SymbolRow.RareRange;
    case EMortisRuneGrade::Epic:      return SymbolRow.EpicRange;
    case EMortisRuneGrade::Legendary: return SymbolRow.LegendaryRange;
    default:                    return SymbolRow.CommonRange;
    }
}

bool UMortisRuneDatabaseSubsystem::PickRandomGrade(const FMortisGradeWeights& InWeights, EMortisRuneGrade& OutGrade) const
{
    const float TotalWeight =
        InWeights.Common +
        InWeights.Rare +
        InWeights.Epic +
        InWeights.Legendary;

    if (TotalWeight <= 0.f)
        return false;

    const float Roll = FMath::FRandRange(0.f, TotalWeight);

    float AccWeight = InWeights.Common;
    if (Roll <= AccWeight)
    {
        OutGrade = EMortisRuneGrade::Common;
        return true;
    }

    AccWeight += InWeights.Rare;
    if (Roll <= AccWeight)
    {
        OutGrade = EMortisRuneGrade::Rare;
        return true;
    }

    AccWeight += InWeights.Epic;
    if (Roll <= AccWeight)
    {
        OutGrade = EMortisRuneGrade::Epic;
        return true;
    }

    OutGrade = EMortisRuneGrade::Legendary;
    return true;
}
