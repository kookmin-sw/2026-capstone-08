// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "MortisIncomingDamageExecCalc.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisIncomingDamageExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UMortisIncomingDamageExecCalc();
	// UGameplayEffectExecutionCalculation Override
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
