// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/MortisAttributeSet.h"
#include "MortisPlayerAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisPlayerAttributeSet : public UMortisAttributeSet
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, MaxStamina)

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	FGameplayAttributeData CurrentStamina;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, CurrentStamina)

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, Strength)

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	FGameplayAttributeData Dexterity;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, Dexterity)

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, Intelligence)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData WeaponDamage;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, WeaponDamage)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData StrCoef;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, StrCoef)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DexCoef;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, DexCoef)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData IntCoef;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, IntCoef)

	UMortisPlayerAttributeSet();

	// UAttributeSet Override
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
