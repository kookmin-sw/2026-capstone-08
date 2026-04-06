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

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	FGameplayAttributeData StaminaRegen;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, StaminaRegen)

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	FGameplayAttributeData StaminaReductionRate;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, StaminaReductionRate)

	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, MaxMana)

	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	FGameplayAttributeData CurrentMana;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, CurrentMana)

	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	FGameplayAttributeData ManaRegen;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, ManaRegen)

	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	FGameplayAttributeData ManaReductionRate;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, ManaReductionRate)



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



	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData AllTypeBonus;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, AllTypeBonus)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData PierceBonus;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, PierceBonus)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData BluntBonus;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, BluntBonus)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData SlashBonus;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, SlashBonus)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData MagicBonus;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, MagicBonus)



	UPROPERTY(BlueprintReadOnly, Category = "Speed")
	FGameplayAttributeData LightAttackSpeed;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, LightAttackSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Speed")
	FGameplayAttributeData WeaponSkillSpeed;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, WeaponSkillSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Speed")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, MoveSpeed)


	UPROPERTY(BlueprintReadOnly, Category = "ETC")
	FGameplayAttributeData RollStaminaReduction;
	ATTRIBUTE_ACCESSORS(UMortisPlayerAttributeSet, RollStaminaReduction)

	UMortisPlayerAttributeSet();

	// UAttributeSet Override
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
