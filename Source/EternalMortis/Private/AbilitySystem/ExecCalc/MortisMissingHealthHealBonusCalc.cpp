// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/MortisMissingHealthHealBonusCalc.h"
#include "AbilitySystem/Attributes/MortisAttributeSet.h"
#include "MortisGameplayTags.h"

UMortisMissingHealthHealBonusCalc::UMortisMissingHealthHealBonusCalc()
{
	CurrentHealthDef = FGameplayEffectAttributeCaptureDefinition(
		UMortisAttributeSet::GetCurrentHealthAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	MaxHealthDef = FGameplayEffectAttributeCaptureDefinition(
		UMortisAttributeSet::GetMaxHealthAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	);

	RelevantAttributesToCapture.Add(CurrentHealthDef);
	RelevantAttributesToCapture.Add(MaxHealthDef);
}

float UMortisMissingHealthHealBonusCalc::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float CurrentHealth = 0.f;
	float MaxHealth = 1.f;

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	GetCapturedAttributeMagnitude(CurrentHealthDef, Spec, EvaluateParameters, CurrentHealth);
	GetCapturedAttributeMagnitude(MaxHealthDef, Spec, EvaluateParameters, MaxHealth);

	if (MaxHealth <= 0.f)
		return 0.f;

	const float HealthRate = FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f);
	const float MissingHealthRate = 1.f - HealthRate;

	// 체력이 0%일 때 도달할 최대 받는 회복량 증가율
	const float MaxHealingReceivedBonusRate = Spec.GetSetByCallerMagnitude(
		MortisGameplayTags::Data_RuneValue,
		false,
		0.f
	);

	return MaxHealingReceivedBonusRate * MissingHealthRate;
}
