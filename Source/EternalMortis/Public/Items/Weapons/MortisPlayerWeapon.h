// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/MortisWeaponBase.h"
#include "Types/MortisStructTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "MortisPlayerWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisPlayerWeapon : public AMortisWeaponBase
{
	GENERATED_BODY()

public:
	AMortisPlayerWeapon();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|WeaponData")
	FMortisPlayerWeaponData PlayerWeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mortis|Weapon")
	FActiveGameplayEffectHandle WeaponDamageEffectHandle;
	
	UFUNCTION (BlueprintCallable)
	void AssignWeaponAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle> SpecHandlesToGrant);

	UFUNCTION(BlueprintPure)
	TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;


private:
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
};
