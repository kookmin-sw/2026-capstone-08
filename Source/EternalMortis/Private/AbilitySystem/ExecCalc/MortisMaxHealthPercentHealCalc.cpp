// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/MortisMaxHealthPercentHealCalc.h"
#include "AbilitySystem/Attributes/MortisAttributeSet.h"
#include "MortisGameplayTags.h"

UMortisMaxHealthPercentHealCalc::UMortisMaxHealthPercentHealCalc()
{
    MaxHealthDef = FGameplayEffectAttributeCaptureDefinition(
        UMortisAttributeSet::GetMaxHealthAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        false
    );

    RelevantAttributesToCapture.Add(MaxHealthDef);
}

float UMortisMaxHealthPercentHealCalc::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    FAggregatorEvaluateParameters EvalParams;
    EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    float MaxHealth = 0.f;
    GetCapturedAttributeMagnitude(MaxHealthDef, Spec, EvalParams, MaxHealth);
    MaxHealth = FMath::Max(0.f, MaxHealth);

    const FGameplayTag HealPercentTag = MortisGameplayTags::Data_Cost_Health;

    const float HealPercent = Spec.GetSetByCallerMagnitude(HealPercentTag, false, 0.f);

    return MaxHealth * HealPercent;
}
