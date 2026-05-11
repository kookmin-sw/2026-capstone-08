// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MortisMissingHealthResistance.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisMissingHealthResistance : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMortisMissingHealthResistance();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition CurrentHealthDef;
	FGameplayEffectAttributeCaptureDefinition MaxHealthDef;
};
