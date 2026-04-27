// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "MortisSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(SaveGame)
	int32 MemoryFragments = 0;

	UPROPERTY(SaveGame)
	TArray<FGameplayTag> UnlockedExperienceTags;

	UPROPERTY(SaveGame)
	FGameplayTag SelectedExperienceTag;

	UPROPERTY(SaveGame)
	int32 RuneSlotUpgradeLevel = 0;

	UPROPERTY(SaveGame)
	bool bHasCompletedTutorial = false;
};
