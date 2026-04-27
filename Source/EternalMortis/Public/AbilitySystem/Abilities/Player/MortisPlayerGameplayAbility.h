// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MortisGameplayAbility.h"
#include "Types/MortisStructTypes.h"
#include "MortisPlayerGameplayAbility.generated.h"

class AMortisPlayerCharacter;
class AMortisPlayerController;
class UMortisPlayerCombatComponent;
class UMortisPlayerUIComponent;
class UMeshComponent;

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

	UFUNCTION(BlueprintPure, Category = "Mortis|Ability")
	UMortisPlayerUIComponent* GetMortisPlayerUIComponent();

	UFUNCTION(BlueprintPure, Category = "Mortis|Ability")
	FGameplayEffectSpecHandle MakePlayerBaseDamageUpdateEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FMortisPlayerWeaponData& WeaponData, const float AttackScale, FGameplayTag AttackType);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability")
	float CalculateStaminaCost(float BaseCost, float AdditionalReduceRate = 0.0f) const;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability")
	bool CheckStaminaCost(float BaseCost, float& OutFinalCost, float AdditionalReduceRate = 0.0f) const;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability")
	float CalculateManaCost(float BaseCost, float AdditionalReduceRate = 0.0f) const;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability")
	bool CheckManaCost(float BaseCost, float& OutFinalCost, float AdditionalReduceRate = 0.0f) const;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability")
	void BroadcastEquippedWeaponMeshChanged(UMeshComponent* NewWeaponMesh);

private:
	TWeakObjectPtr<AMortisPlayerCharacter> CachedMortisPlayerCharacter;
	TWeakObjectPtr<AMortisPlayerController> CachedMortisPlayerController;

};
