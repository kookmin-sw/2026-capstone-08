// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/MortisWeaponBase.h"
#include "Types/MortisStructTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "MortisPlayerWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisPlayerWeapon : public AMortisWeaponBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	FMortisPlayerWeaponData PlayerWeaponData;

	UFUNCTION (BlueprintCallable)
	void AssignWeaponAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle> SpecHandelsToGrant);

	UFUNCTION(BlueprintPure)
	TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandels() const;

private:
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
};
