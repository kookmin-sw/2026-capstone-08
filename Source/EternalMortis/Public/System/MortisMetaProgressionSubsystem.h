// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/MortisMetaProgressionDataTypes.h"
#include "MortisGameplayTags.h"
#include "MortisMetaProgressionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortisOnMemoryFragmentsChanged, int32, NewValue);

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisMetaProgressionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 기억파편
	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression")
	void AddMemoryFragments(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression")
	int32 GetMemoryFragments() const { return MemoryFragments; }

	// 경험
	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression")
	bool UnlockExperience(FGameplayTag ExperienceTag);

	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression")
	bool SelectExperience(FGameplayTag ExperienceTag);

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression")
	bool IsExperienceUnlocked(FGameplayTag ExperienceTag) const;

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression")
	FGameplayTag GetSelectedExperienceTag() const { return SelectedExperienceTag; }

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression")
	bool GetExperienceRowByTag(FGameplayTag ExperienceTag, FMortisExperienceRow& OutRow) const;

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression")
	bool GetSelectedExperienceRow(FMortisExperienceRow& OutRow) const;

	const FMortisExperienceRow* FindExperienceRow(FGameplayTag ExperienceTag) const;
	const FMortisExperienceRow* FindSelectedExperienceRow() const;

	// 룬 슬롯 확장
	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression")
	bool UpgradeRuneSlot();

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression")
	int32 GetUnlockedRuneSlotCount() const;

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression")
	int32 GetMaxRuneSlotCount() const { return MaxRuneSlotCount; }

	// 튜토리얼
	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression")
	void MarkTutorialCompleted();

	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression")
	void ResetTutorialProgress();

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression")
	bool HasCompletedTutorial() const { return bHasCompletedTutorial; }

	UPROPERTY(BlueprintAssignable, Category = "Mortis|MetaProgression")
	FMortisOnMemoryFragmentsChanged OnMemoryFragmentsChanged;

private:
	void BuildCaches();

	void LoadMetaProgression();
	void SaveMetaProgression() const;

private:
	UPROPERTY()
	TObjectPtr<UDataTable> ExperienceTable = nullptr;

	UPROPERTY()
	TMap<FGameplayTag, FMortisExperienceRow> ExperienceMap;

	UPROPERTY()
	int32 MemoryFragments = 0;

	UPROPERTY()
	TArray<FGameplayTag> UnlockedExperienceTags;

	UPROPERTY()
	FGameplayTag SelectedExperienceTag = MortisGameplayTags::Experience_Test;

	UPROPERTY()
	int32 RuneSlotUpgradeLevel = 0;

	UPROPERTY()
	bool bHasCompletedTutorial = false;

	UPROPERTY()
	int32 BaseRuneSlotCount = 6;

	UPROPERTY()
	int32 MaxRuneSlotCount = 10;

	UPROPERTY()
	TArray<int32> RuneSlotUpgradeCosts = { 300, 500, 700, 1000 };

	UPROPERTY(EditAnywhere)
	FString SaveSlotName = TEXT("MortisSave");

	UPROPERTY()
	int32 SaveUserIndex = 0;
};
