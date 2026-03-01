// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Types/MortisStructTypes.h"
#include "MortisAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	// Player ��
	void OnAbilityInputPressed(const FGameplayTag& InputTag);
	void OnAbilityInputReleased(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability", meta = (ApplyLevel = "1"))
	void GrantPlayerWeaponAbilities(const TArray<FMortisAbilityInputBinding>& DefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& GrantedAbilitySpecHandles);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability")
	void RemoveGrantedPlayerWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& SpecHandlesToRemove);

	// Shared
	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
};
