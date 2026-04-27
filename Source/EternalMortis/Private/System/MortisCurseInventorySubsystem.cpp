// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MortisCurseInventorySubsystem.h"
#include "System/MortisCurseDatabaseSubsystem.h"

bool UMortisCurseInventorySubsystem::GenerateAndAddCurse(FMortisCurseInstance& OutCurse)
{
	OutCurse = FMortisCurseInstance();

	UGameInstance* GI = GetGameInstance();
	if (!GI)
		return false;

	UMortisCurseDatabaseSubsystem* CurseDB = GI->GetSubsystem<UMortisCurseDatabaseSubsystem>();
	if (!CurseDB)
		return false;

	const FMortisCurseInstance NewCurse = CurseDB->GenerateCurse();

	if (!NewCurse.CurseTag.IsValid())
		return false;

	if (!AddCurse(NewCurse))
		return false;

	OutCurse = NewCurse;
	return true;
}

bool UMortisCurseInventorySubsystem::AddCurse(const FMortisCurseInstance& Curse)
{
	if (!Curse.CurseTag.IsValid())
		return false;

	OwnedCurses.Add(Curse);
	OnCurseAdded.Broadcast(Curse);

	return true;
}

bool UMortisCurseInventorySubsystem::RemoveCurseById(FGuid CurseInstanceId)
{
	for (int32 i = OwnedCurses.Num() - 1; i >= 0; --i)
	{
		if (OwnedCurses[i].InstanceId != CurseInstanceId)
			continue;

		const FMortisCurseInstance RemovedCurse = OwnedCurses[i];
		OwnedCurses.RemoveAt(i);

		OnCurseRemoved.Broadcast(RemovedCurse);
		return true;
	}

	return false;
}
