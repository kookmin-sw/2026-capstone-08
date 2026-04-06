// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MortisWeaponDatabaseSubsystem.h"
#include "System/MortisGameDataSettings.h"

#include "MortisDebugHelper.h"

void UMortisWeaponDatabaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    const UMortisGameDataSettings* Settings = GetDefault<UMortisGameDataSettings>();
    check(Settings);

    WeaponTable = Settings->WeaponTable.LoadSynchronous();
    WeaponDropRuleTable = Settings->WeaponDropRuleTable.LoadSynchronous();
    WeaponGradeStyleTable = Settings->WeaponGradeStyleTable.LoadSynchronous();

    checkf(WeaponTable, TEXT("WeaponTable is not set in MortisGameDataSettings."));
    checkf(WeaponDropRuleTable, TEXT("WeaponDropRuleTable is not set in MortisGameDataSettings."));
    checkf(WeaponGradeStyleTable, TEXT("WeaponGradeStyleTable is not set in MortisGameDataSettings."));

    BuildWeaponCaches();
    BuildDropRuleCaches();
}

bool UMortisWeaponDatabaseSubsystem::GetWeaponRowByTag(FGameplayTag WeaponTag, FMortisWeaponRow& OutWeaponRow) const
{
    if (const FMortisWeaponRow* FoundRow = WeaponRowsByTag.Find(WeaponTag))
    {
        OutWeaponRow = *FoundRow;
        return true;
    }

    return false;
}

bool UMortisWeaponDatabaseSubsystem::GenerateWeaponByTag(FGameplayTag WeaponTag, FMortisWeaponRow& OutWeaponRow) const
{
    return GetWeaponRowByTag(WeaponTag, OutWeaponRow);
}

bool UMortisWeaponDatabaseSubsystem::GenerateRandomWeaponForFloor(int32 Floor, FMortisWeaponRow& OutWeaponRow) const
{
    const FMortisWeaponDropRuleRow* DropRule = GetDropRuleForFloor(Floor);
    if (!DropRule)
        return false;

    EMortisWeaponGrade PickedGrade;
    if (!PickRandomGrade(DropRule->GradeWeights, PickedGrade))
        return false;

    return GenerateRandomWeaponByGrade(PickedGrade, OutWeaponRow);
}

const FMortisWeaponDropRuleRow* UMortisWeaponDatabaseSubsystem::GetDropRuleForFloor(int32 Floor) const
{
    if (Floor <= 0 || DropRulesByFloor.Num() == 0)
        return nullptr;

    if (const FMortisWeaponDropRuleRow* ExactRule = DropRulesByFloor.Find(Floor))
        return ExactRule;

    return nullptr;
}

bool UMortisWeaponDatabaseSubsystem::GenerateRandomWeaponByGrade(EMortisWeaponGrade Grade, FMortisWeaponRow& OutWeaponRow) const
{
    const TArray<FMortisWeaponRow>* GradeRows = WeaponRowsByGrade.Find(Grade);
    if (!GradeRows || GradeRows->Num() == 0)
        return false;

    const int32 Index = FMath::RandRange(0, GradeRows->Num() - 1);
    OutWeaponRow = (*GradeRows)[Index];
    return true;
}

bool UMortisWeaponDatabaseSubsystem::GetWeaponGradeStyleByHandle(const FDataTableRowHandle& InHandle, FMortisWeaponGradeStyleRow& OutStyleRow) const
{
    if (const FMortisWeaponGradeStyleRow* FoundRow = InHandle.GetRow<FMortisWeaponGradeStyleRow>(TEXT("GetWeaponGradeStyleByHandle")))
    {
        OutStyleRow = *FoundRow;
        return true;
    }

    return false;
}

void UMortisWeaponDatabaseSubsystem::BuildWeaponCaches()
{
    WeaponRowsByTag.Empty();
    WeaponRowsByGrade.Empty();

    checkf(WeaponTable, TEXT("WeaponTable is not Correct!"));
    checkf(WeaponGradeStyleTable, TEXT("WeaponGradeStyleTable is not Correct!"));

    TArray<FMortisWeaponRow*> AllRows;
    WeaponTable->GetAllRows(TEXT("BuildWeaponCaches"), AllRows);

    for (const FMortisWeaponRow* Row : AllRows)
    {
        if (!Row || !Row->IsValid())
        {
            continue;
        }

        FMortisWeaponRow CachedRow = *Row;
        CachedRow.GradeStyleRow.DataTable = WeaponGradeStyleTable;
        CachedRow.GradeStyleRow.RowName = FName(*StaticEnum<EMortisWeaponGrade>()->GetNameStringByValue((int64)CachedRow.Grade));

        WeaponRowsByTag.Add(CachedRow.WeaponTag, CachedRow);
        WeaponRowsByGrade.FindOrAdd(CachedRow.Grade).Add(CachedRow);
    }
}

void UMortisWeaponDatabaseSubsystem::BuildDropRuleCaches()
{
    DropRulesByFloor.Empty();

    checkf(WeaponDropRuleTable, TEXT("WeaponDropRuleTable is not Correct!"));

    TArray<FMortisWeaponDropRuleRow*> AllRows;
    WeaponDropRuleTable->GetAllRows(TEXT("BuildDropRuleCaches"), AllRows);

    for (const FMortisWeaponDropRuleRow* Row : AllRows)
    {
        if (!Row || !Row->IsValid())
            continue;

        DropRulesByFloor.Add(Row->Floor, *Row);
    }
}

bool UMortisWeaponDatabaseSubsystem::PickRandomGrade(const FMortisWeaponGradeWeights& InWeights, EMortisWeaponGrade& OutGrade) const
{
    const float TotalWeight =
        InWeights.Common +
        InWeights.Rare +
        InWeights.Epic +
        InWeights.Legendary;

    checkf(TotalWeight > 0.f, TEXT("Weapon grade weights total must be greater than 0."));

    const float Roll = FMath::FRandRange(0.f, TotalWeight);

    float AccWeight = InWeights.Common;
    if (Roll <= AccWeight)
    {
        OutGrade = EMortisWeaponGrade::Common;
        return true;
    }

    AccWeight += InWeights.Rare;
    if (Roll <= AccWeight)
    {
        OutGrade = EMortisWeaponGrade::Rare;
        return true;
    }

    AccWeight += InWeights.Epic;
    if (Roll <= AccWeight)
    {
        OutGrade = EMortisWeaponGrade::Epic;
        return true;
    }

    OutGrade = EMortisWeaponGrade::Legendary;
    return true;
}

bool UMortisWeaponDatabaseSubsystem::HasAnyWeaponInGrade(EMortisWeaponGrade Grade) const
{
    const TArray<FMortisWeaponRow>* GradeRows = WeaponRowsByGrade.Find(Grade);
    return GradeRows && GradeRows->Num() > 0;
}
