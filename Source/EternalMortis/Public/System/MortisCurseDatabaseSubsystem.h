// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/MortisCurseDataTypes.h"
#include "MortisCurseDatabaseSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisCurseDatabaseSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	FMortisCurseInstance GenerateCurse() const;

	UFUNCTION(BlueprintCallable)
	FMortisCurseInstance GenerateCurseWithTag(FGameplayTag CurseTag) const;

	const FMortisCurseRow* GetCurseRow(FGameplayTag CurseTag) const;

private:
	void BuildCaches();

private:
	UPROPERTY()
	TObjectPtr<UDataTable> CurseTable = nullptr;

	UPROPERTY()
	TMap<FGameplayTag, FMortisCurseRow> CurseMap;

	UPROPERTY()
	TArray<FGameplayTag> ValidCurseTags;
};
