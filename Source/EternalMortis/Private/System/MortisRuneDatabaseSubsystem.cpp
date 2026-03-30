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
    DropRuleTable = Settings->RuneDropRuleTable.LoadSynchronous();

	BuildCaches();
}

void UMortisRuneDatabaseSubsystem::BuildCaches()
{
    RuneSymbolMap.Empty();
    RuneSetMap.Empty();
    DropRules.Empty();

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
        }
    }

    if (DropRuleTable)
    {
        TArray<FMortisRuneDropRuleRow*> Rows;
        DropRuleTable->GetAllRows(TEXT("DropRuleTable"), Rows);

        for (FMortisRuneDropRuleRow* Row : Rows)
        {
            if (!Row) continue;
            DropRules.Add(*Row);
        }
    }
}

FMortisRuneInstance UMortisRuneDatabaseSubsystem::GenerateRune(int32 CurrentFloor) const
{
    FMortisRuneInstance NewRune;
    NewRune.InstanceId = FGuid::NewGuid();

    const FMortisRuneDropRuleRow* Rule = GetDropRuleForFloor(CurrentFloor);
    if (!Rule) return NewRune;

    // 세트 선택
    const FMortisRuneSetWeight* SetPick = GetWeightedRandomEntry<FMortisRuneSetWeight>(Rule->SetWeights, 
        [](const FMortisRuneSetWeight& Entry) { return Entry.Weight; });
    if (!SetPick) return NewRune;

    NewRune.SetTag = SetPick->SetTag;

    // 세트 조회
    const FMortisRuneSetRow* SetRow = GetRuneSetRow(NewRune.SetTag);
    if (!SetRow || SetRow->AllowedSymbols.Num() <= 0)return NewRune;

    // 세트 내 룬 문자
    const EMortisRuneSymbol* SymbolPick = GetUniformRandomEntry<EMortisRuneSymbol>(SetRow->AllowedSymbols);
    if (!SymbolPick || *SymbolPick == EMortisRuneSymbol::None) return NewRune;

    NewRune.SymbolType = *SymbolPick;

    // 등급 선택
    const FMortisRuneGradeWeight* GradePick = GetWeightedRandomEntry<FMortisRuneGradeWeight>(Rule->GradeWeights,
        [](const FMortisRuneGradeWeight& Entry) { return Entry.Weight; });
    if (!GradePick)return NewRune;

    NewRune.Grade = GradePick->Grade;

    // 룬 문자 데이터 조회
    const FMortisRuneSymbolRow* SymbolRow = GetRuneSymbolRow(NewRune.SymbolType);
    if (!SymbolRow) return NewRune;

    // 값 랜덤
    const FMortisRuneValueRange Range = GetValueRangeByGrade(*SymbolRow, NewRune.Grade);
    NewRune.RolledValue = floor(FMath::FRandRange(Range.MinValue, Range.MaxValue));

    return NewRune;
}

FMortisRuneInstance UMortisRuneDatabaseSubsystem::GenerateRuneWithTag(FGameplayTag SetTag, EMortisRuneGrade Grade) const
{
    FMortisRuneInstance NewRune;

    const FMortisRuneSetRow* SetRow = GetRuneSetRow(SetTag);
    if (!SetRow || SetRow->AllowedSymbols.Num() <= 0)
        return NewRune;

    const EMortisRuneSymbol* SymbolPick = GetUniformRandomEntry<EMortisRuneSymbol>(SetRow->AllowedSymbols);
    if (!SymbolPick || *SymbolPick == EMortisRuneSymbol::None)
        return NewRune;

    const FMortisRuneSymbolRow* SymbolRow = GetRuneSymbolRow(*SymbolPick);
    if (!SymbolRow)
        return NewRune;

    const FMortisRuneValueRange Range = GetValueRangeByGrade(*SymbolRow, Grade);

    NewRune.InstanceId = FGuid::NewGuid();
    NewRune.SetTag = SetTag;
    NewRune.SymbolType = *SymbolPick;
    NewRune.Grade = Grade;
    NewRune.RolledValue = FMath::FloorToFloat(FMath::FRandRange(Range.MinValue, Range.MaxValue));

    return NewRune;
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

    return NewRune;
}

const FMortisRuneSymbolRow* UMortisRuneDatabaseSubsystem::GetRuneSymbolRow(EMortisRuneSymbol SymbolType) const
{
	return RuneSymbolMap.Find(SymbolType);
}

const FMortisRuneSetRow* UMortisRuneDatabaseSubsystem::GetRuneSetRow(FGameplayTag SetTag) const
{
	return RuneSetMap.Find(SetTag);
}

const FMortisRuneDropRuleRow* UMortisRuneDatabaseSubsystem::GetDropRuleForFloor(int32 Floor) const
{
    for (const FMortisRuneDropRuleRow& Rule : DropRules)
    {
        if (Floor >= Rule.MinFloor && Floor <= Rule.MaxFloor)
            return &Rule;
    }

    return nullptr;
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