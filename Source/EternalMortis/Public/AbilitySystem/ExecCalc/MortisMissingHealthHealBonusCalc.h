// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MortisMissingHealthHealBonusCalc.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisMissingHealthHealBonusCalc : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMortisMissingHealthHealBonusCalc();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition CurrentHealthDef;
	FGameplayEffectAttributeCaptureDefinition MaxHealthDef;
};
