// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MortisMetaProgressionSubsystem.h"
#include "System/MortisGameDataSettings.h"
#include "Save/MortisSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UMortisMetaProgressionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UMortisGameDataSettings* Settings = GetDefault<UMortisGameDataSettings>();
	check(Settings);

	ExperienceTable = Settings->ExperienceTable.LoadSynchronous();
	checkf(ExperienceTable, TEXT("ExperienceTable is not set in MortisGameDataSettings."));

	BuildCaches();
	LoadMetaProgression();
}

void UMortisMetaProgressionSubsystem::BuildCaches()
{
	ExperienceMap.Empty();

	if (!ExperienceTable)
		return;

	TArray<FMortisExperienceRow*> Rows;
	ExperienceTable->GetAllRows(TEXT("ExperienceTable"), Rows);

	for (FMortisExperienceRow* Row : Rows)
	{
		if (!Row)
			continue;

		ExperienceMap.Add(Row->ExperienceTag, *Row);
	}
}

void UMortisMetaProgressionSubsystem::AddMemoryFragments(int32 Amount)
{
	if (Amount <= 0)
		return;

	MemoryFragments += Amount;

	SaveMetaProgression();

	OnMemoryFragmentsChanged.Broadcast(MemoryFragments);
}

bool UMortisMetaProgressionSubsystem::UnlockExperience(FGameplayTag ExperienceTag)
{
	const FMortisExperienceRow* Row = FindExperienceRow(ExperienceTag);
	if (!Row)
		return false;

	if (IsExperienceUnlocked(ExperienceTag))
		return false;

	if (MemoryFragments < Row->UnlockCost)
		return false;

	MemoryFragments -= Row->UnlockCost;
	UnlockedExperienceTags.AddUnique(ExperienceTag);

	if (!SelectedExperienceTag.IsValid())
	{
		SelectedExperienceTag = ExperienceTag;
	}

	SaveMetaProgression();

	OnMemoryFragmentsChanged.Broadcast(MemoryFragments);

	return true;
}

bool UMortisMetaProgressionSubsystem::SelectExperience(FGameplayTag ExperienceTag)
{
	if (!IsExperienceUnlocked(ExperienceTag))
		return false;

	SelectedExperienceTag = ExperienceTag;

	SaveMetaProgression();

	return true;
}

bool UMortisMetaProgressionSubsystem::IsExperienceUnlocked(FGameplayTag ExperienceTag) const
{
	return ExperienceTag.IsValid() && UnlockedExperienceTags.Contains(ExperienceTag);
}

bool UMortisMetaProgressionSubsystem::GetExperienceRowByTag(FGameplayTag ExperienceTag, FMortisExperienceRow& OutRow) const
{
	const FMortisExperienceRow* Row = FindExperienceRow(ExperienceTag);
	if (!Row)
		return false;

	OutRow = *Row;
	return true;
}

bool UMortisMetaProgressionSubsystem::GetSelectedExperienceRow(FMortisExperienceRow& OutRow) const
{
	const FMortisExperienceRow* Row = FindSelectedExperienceRow();
	if (!Row)
		return false;

	OutRow = *Row;
	return true;
}

const FMortisExperienceRow* UMortisMetaProgressionSubsystem::FindExperienceRow(FGameplayTag ExperienceTag) const
{
	if (!ExperienceTag.IsValid())
		return nullptr;

	return ExperienceMap.Find(ExperienceTag);
}

const FMortisExperienceRow* UMortisMetaProgressionSubsystem::FindSelectedExperienceRow() const
{
	if (!SelectedExperienceTag.IsValid())
		return nullptr;

	return FindExperienceRow(SelectedExperienceTag);
}

bool UMortisMetaProgressionSubsystem::UpgradeRuneSlot()
{
	if (GetUnlockedRuneSlotCount() >= MaxRuneSlotCount)
		return false;

	if (!RuneSlotUpgradeCosts.IsValidIndex(RuneSlotUpgradeLevel))
		return false;

	const int32 Cost = RuneSlotUpgradeCosts[RuneSlotUpgradeLevel];

	if (MemoryFragments < Cost)
		return false;

	MemoryFragments -= Cost;
	RuneSlotUpgradeLevel++;

	SaveMetaProgression();

	OnMemoryFragmentsChanged.Broadcast(MemoryFragments);

	return true;
}

int32 UMortisMetaProgressionSubsystem::GetUnlockedRuneSlotCount() const
{
	return FMath::Clamp(
		BaseRuneSlotCount + RuneSlotUpgradeLevel,
		BaseRuneSlotCount,
		MaxRuneSlotCount
	);
}

void UMortisMetaProgressionSubsystem::MarkTutorialCompleted()
{
	if (bHasCompletedTutorial)
		return;

	bHasCompletedTutorial = true;

	SaveMetaProgression();
}

void UMortisMetaProgressionSubsystem::ResetTutorialProgress()
{
	bHasCompletedTutorial = false;

	SaveMetaProgression();
}

void UMortisMetaProgressionSubsystem::SaveMetaProgression() const
{
	UMortisSaveGame* SaveGame =
		Cast<UMortisSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UMortisSaveGame::StaticClass())
		);

	if (!SaveGame)
		return;

	SaveGame->MemoryFragments = MemoryFragments;
	SaveGame->UnlockedExperienceTags = UnlockedExperienceTags;
	SaveGame->SelectedExperienceTag = SelectedExperienceTag;
	SaveGame->RuneSlotUpgradeLevel = RuneSlotUpgradeLevel;
	SaveGame->bHasCompletedTutorial = bHasCompletedTutorial;

	UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, SaveUserIndex);
}

void UMortisMetaProgressionSubsystem::LoadMetaProgression()
{
	if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
	{
		SaveMetaProgression();
		return;
	}

	UMortisSaveGame* SaveGame =
		Cast<UMortisSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex)
		);

	if (!SaveGame)
		return;

	MemoryFragments = SaveGame->MemoryFragments;
	UnlockedExperienceTags = SaveGame->UnlockedExperienceTags;
	SelectedExperienceTag = SaveGame->SelectedExperienceTag;
	RuneSlotUpgradeLevel = SaveGame->RuneSlotUpgradeLevel;
	bHasCompletedTutorial = SaveGame->bHasCompletedTutorial;

	RuneSlotUpgradeLevel = FMath::Clamp(
		RuneSlotUpgradeLevel,
		0,
		MaxRuneSlotCount - BaseRuneSlotCount
	);

	for (int32 i = UnlockedExperienceTags.Num() - 1; i >= 0; --i)
	{
		if (!FindExperienceRow(UnlockedExperienceTags[i]))
		{
			UnlockedExperienceTags.RemoveAt(i);
		}
	}

	if (!IsExperienceUnlocked(SelectedExperienceTag))
	{
		SelectedExperienceTag = FGameplayTag();
	}
}