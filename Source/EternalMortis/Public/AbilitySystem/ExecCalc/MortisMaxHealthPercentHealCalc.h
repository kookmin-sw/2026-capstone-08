// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MortisMaxHealthPercentHealCalc.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisMaxHealthPercentHealCalc : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMortisMaxHealthPercentHealCalc();

	// UGameplayModMagnitudeCalculation Override
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	FGameplayEffectAttributeCaptureDefinition MaxHealthDef;
};
