// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MortisCurseDatabaseSubsystem.h"
#include "System/MortisGameDataSettings.h"

void UMortisCurseDatabaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UMortisGameDataSettings* Settings = GetDefault<UMortisGameDataSettings>();
	check(Settings);

	CurseTable = Settings->CurseTable.LoadSynchronous();
	checkf(CurseTable, TEXT("CurseTable is not set in MortisGameDataSettings."));

	BuildCaches();
}

FMortisCurseInstance UMortisCurseDatabaseSubsystem::GenerateCurse() const
{
	if (ValidCurseTags.IsEmpty())
		return FMortisCurseInstance();

	const int32 Index = FMath::RandRange(0, ValidCurseTags.Num() - 1);
	return GenerateCurseWithTag(ValidCurseTags[Index]);
}

FMortisCurseInstance UMortisCurseDatabaseSubsystem::GenerateCurseWithTag(FGameplayTag CurseTag) const
{
	FMortisCurseInstance NewCurse;

	const FMortisCurseRow* Row = GetCurseRow(CurseTag);
	if (!Row)
		return NewCurse;

	NewCurse.InstanceId = FGuid::NewGuid();
	NewCurse.CurseTag = CurseTag;
	NewCurse.RolledValue = FMath::FRandRange(Row->ValueRange.MinValue, Row->ValueRange.MaxValue);

	return NewCurse;
}

const FMortisCurseRow* UMortisCurseDatabaseSubsystem::GetCurseRow(FGameplayTag CurseTag) const
{
	return CurseMap.Find(CurseTag);
}

void UMortisCurseDatabaseSubsystem::BuildCaches()
{
	CurseMap.Empty();
	ValidCurseTags.Empty();

	TArray<FMortisCurseRow*> Rows;
	CurseTable->GetAllRows(TEXT("CurseTable"), Rows);

	for (FMortisCurseRow* Row : Rows)
	{
		if (!Row)
			continue;

		CurseMap.Add(Row->CurseTag, *Row);
		ValidCurseTags.Add(Row->CurseTag);
	}
}