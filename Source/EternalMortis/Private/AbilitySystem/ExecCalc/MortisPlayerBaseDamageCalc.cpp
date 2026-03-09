	// Fill out your copyright notice in the Description page of Project Settings.


	#include "AbilitySystem/ExecCalc/MortisPlayerBaseDamageCalc.h"
	#include "AbilitySystem/Attributes/MortisPlayerAttributeSet.h"

	UMortisPlayerBaseDamageCalc::UMortisPlayerBaseDamageCalc()
	{
		StrengthDef = FGameplayEffectAttributeCaptureDefinition(UMortisPlayerAttributeSet::GetStrengthAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		DexterityDef = FGameplayEffectAttributeCaptureDefinition(UMortisPlayerAttributeSet::GetDexterityAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		IntelligenceDef = FGameplayEffectAttributeCaptureDefinition(UMortisPlayerAttributeSet::GetIntelligenceAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);

		WeaponDamageDef = FGameplayEffectAttributeCaptureDefinition(UMortisPlayerAttributeSet::GetWeaponDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		StrCoefDef = FGameplayEffectAttributeCaptureDefinition(UMortisPlayerAttributeSet::GetStrCoefAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		DexCoefDef = FGameplayEffectAttributeCaptureDefinition(UMortisPlayerAttributeSet::GetDexCoefAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		IntCoefDef = FGameplayEffectAttributeCaptureDefinition(UMortisPlayerAttributeSet::GetIntCoefAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);

		RelevantAttributesToCapture.Add(StrengthDef);
		RelevantAttributesToCapture.Add(DexterityDef);
		RelevantAttributesToCapture.Add(IntelligenceDef);

		RelevantAttributesToCapture.Add(WeaponDamageDef);
		RelevantAttributesToCapture.Add(StrCoefDef);
		RelevantAttributesToCapture.Add(DexCoefDef);
		RelevantAttributesToCapture.Add(IntCoefDef);
	}

	float UMortisPlayerBaseDamageCalc::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
	{
		FAggregatorEvaluateParameters Params;
		Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
		Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

		float Str = 0.0f;
		float Dex = 0.0f;
		float Int = 0.0f;

		float WeaponDamage = 0.0f;
		float StrCoef = 0.0f;
		float DexCoef = 0.0f;
		float IntCoef = 0.0f;
	
		// constexpr는 더 극적인 최적화가 가능하다고 하네요
		constexpr float K = 100.0f;
		constexpr float P = 100.0f;

		GetCapturedAttributeMagnitude(StrengthDef, Spec, Params, Str);
		GetCapturedAttributeMagnitude(DexterityDef, Spec, Params, Dex);
		GetCapturedAttributeMagnitude(IntelligenceDef, Spec, Params, Int);

		GetCapturedAttributeMagnitude(WeaponDamageDef, Spec, Params, WeaponDamage);
		GetCapturedAttributeMagnitude(StrCoefDef, Spec, Params, StrCoef);
		GetCapturedAttributeMagnitude(DexCoefDef, Spec, Params, DexCoef);
		GetCapturedAttributeMagnitude(IntCoefDef, Spec, Params, IntCoef);

		const float BaseDamage
			= WeaponDamage * (1 + (
				((K * Str) / (K + Str)) * StrCoef +
				((K * Dex) / (K + Dex)) * DexCoef +
				((K * Int) / (K + Int)) * IntCoef
			) / P);

		return BaseDamage;
	}
