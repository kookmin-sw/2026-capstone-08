// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MortisPlayerBaseDamageCalc.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisPlayerBaseDamageCalc : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMortisPlayerBaseDamageCalc();

	// UGameplayModMagnitudeCalculation Override
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	FGameplayEffectAttributeCaptureDefinition DexterityDef;
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;

	FGameplayEffectAttributeCaptureDefinition WeaponDamageDef;
	FGameplayEffectAttributeCaptureDefinition StrCoefDef;
	FGameplayEffectAttributeCaptureDefinition DexCoefDef;
	FGameplayEffectAttributeCaptureDefinition IntCoefDef;
};
