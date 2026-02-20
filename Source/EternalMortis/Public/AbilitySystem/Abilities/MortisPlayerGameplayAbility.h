// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MortisGameplayAbility.h"
#include "MortisPlayerGameplayAbility.generated.h"

class AMortisPlayerCharacter;
class AMortisPlayerController;
class UMortisPlayerCombatComponent;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisPlayerGameplayAbility : public UMortisGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Mortis|Ability")
	AMortisPlayerCharacter* GetMortisPlayerCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Mortis|Ability")
	AMortisPlayerController* GetMortisPlayerControllerFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Mortis|Ability")
	UMortisPlayerCombatComponent* GetMortisPlayerCombatComponent();


private:
	TWeakObjectPtr<AMortisPlayerCharacter> CachedMortisPlayerCharacter;
	TWeakObjectPtr<AMortisPlayerController> CachedMortisPlayerController;

};
