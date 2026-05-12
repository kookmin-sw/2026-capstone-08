// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/MortisNonWeaponDamageExecCalc.h"
#include "AbilitySystem/Attributes/MortisAttributeSet.h"
#include "AbilitySystem/Attributes/MortisEnemyAttributeSet.h"
#include "MortisGameplayTags.h"

struct FMortisFixedDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistPierce)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistBlunt)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistSlash)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistMagic)

	FMortisFixedDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisAttributeSet, IncomingDamage, Target, false)

			DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisEnemyAttributeSet, ResistPierce, Target, false)
			DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisEnemyAttributeSet, ResistBlunt, Target, false)
			DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisEnemyAttributeSet, ResistSlash, Target, false)
			DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisEnemyAttributeSet, ResistMagic, Target, false)
	}
};

static const FMortisFixedDamageCapture& GetMortisFixedDamageCapture()
{
	static FMortisFixedDamageCapture Capture;
	return Capture;
}

static float GetNonWeaponCapturedOrZero(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FGameplayEffectAttributeCaptureDefinition& Def,
	const FAggregatorEvaluateParameters& Params
)
{
	float CapturedValue = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Def, Params, CapturedValue);
	return CapturedValue;
}

UMortisNonWeaponDamageExecCalc::UMortisNonWeaponDamageExecCalc()
{
	RelevantAttributesToCapture.Add(GetMortisFixedDamageCapture().ResistPierceDef);
	RelevantAttributesToCapture.Add(GetMortisFixedDamageCapture().ResistBluntDef);
	RelevantAttributesToCapture.Add(GetMortisFixedDamageCapture().ResistSlashDef);
	RelevantAttributesToCapture.Add(GetMortisFixedDamageCapture().ResistMagicDef);
}

void UMortisNonWeaponDamageExecCalc::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	const float FixedDamage = EffectSpec.GetSetByCallerMagnitude(
		MortisGameplayTags::Data_Stat_FixedDamage,
		false,
		0.f
	);

	if (FixedDamage <= 0.f)
		return;

	const float ResistPierce = GetNonWeaponCapturedOrZero(
		ExecutionParams,
		GetMortisFixedDamageCapture().ResistPierceDef,
		EvaluateParameters
	);

	const float ResistBlunt = GetNonWeaponCapturedOrZero(
		ExecutionParams,
		GetMortisFixedDamageCapture().ResistBluntDef,
		EvaluateParameters
	);

	const float ResistSlash = GetNonWeaponCapturedOrZero(
		ExecutionParams,
		GetMortisFixedDamageCapture().ResistSlashDef,
		EvaluateParameters
	);

	const float ResistMagic = GetNonWeaponCapturedOrZero(
		ExecutionParams,
		GetMortisFixedDamageCapture().ResistMagicDef,
		EvaluateParameters
	);

	float Resist = 0.f;

	if (EffectSpec.GetDynamicAssetTags().HasTagExact(MortisGameplayTags::Data_AttackType_Pierce))
		Resist = ResistPierce;
	else if (EffectSpec.GetDynamicAssetTags().HasTagExact(MortisGameplayTags::Data_AttackType_Blunt))
		Resist = ResistBlunt;
	else if (EffectSpec.GetDynamicAssetTags().HasTagExact(MortisGameplayTags::Data_AttackType_Slash))
		Resist = ResistSlash;
	else if (EffectSpec.GetDynamicAssetTags().HasTagExact(MortisGameplayTags::Data_AttackType_Magic))
		Resist = ResistMagic;

	const float FinalDamage = FMath::Max(0.f, FixedDamage * (1.f - Resist));

	if (FinalDamage <= 0.f)
		return;

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			GetMortisFixedDamageCapture().IncomingDamageProperty,
			EGameplayModOp::Override,
			FinalDamage
		)
	);
}
