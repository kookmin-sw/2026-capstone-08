// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/MortisCurseDataTypes.h"
#include "MortisCurseInventorySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMortisCurseAdded, const FMortisCurseInstance&, Curse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMortisCurseRemoved, const FMortisCurseInstance&, Curse);

UCLASS()
class ETERNALMORTIS_API UMortisCurseInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool GenerateAndAddCurse(FMortisCurseInstance& OutCurse);

	UFUNCTION(BlueprintCallable)
	bool AddCurse(const FMortisCurseInstance& Curse);

	UFUNCTION(BlueprintCallable)
	bool RemoveCurseById(FGuid CurseInstanceId);

	UFUNCTION(BlueprintPure)
	const TArray<FMortisCurseInstance>& GetOwnedCurses() const { return OwnedCurses; }

	UPROPERTY(BlueprintAssignable)
	FOnMortisCurseAdded OnCurseAdded;

	UPROPERTY(BlueprintAssignable)
	FOnMortisCurseRemoved OnCurseRemoved;

private:
	UPROPERTY()
	TArray<FMortisCurseInstance> OwnedCurses;
};