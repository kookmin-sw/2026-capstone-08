// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/MortisIncomingDamageExecCalc.h"
#include "AbilitySystem/Attributes/MortisEnemyAttributeSet.h"
#include "MortisGameplayTags.h"

struct FMortisDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistPierce)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistBlunt)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistSlash)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistMagic)

	FMortisDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisAttributeSet, BaseDamage, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisAttributeSet, IncomingDamage, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisEnemyAttributeSet, ResistPierce, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisEnemyAttributeSet, ResistBlunt, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisEnemyAttributeSet, ResistSlash, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisEnemyAttributeSet, ResistMagic, Target, false)
	}
};

static const FMortisDamageCapture& GetMortisDamageCapture()
{
	static FMortisDamageCapture MortisDamageCapture;
	return MortisDamageCapture;
}

UMortisIncomingDamageExecCalc::UMortisIncomingDamageExecCalc()
{
	RelevantAttributesToCapture.Add(GetMortisDamageCapture().BaseDamageDef);
	RelevantAttributesToCapture.Add(GetMortisDamageCapture().IncomingDamageDef);
	RelevantAttributesToCapture.Add(GetMortisDamageCapture().ResistPierceDef);
	RelevantAttributesToCapture.Add(GetMortisDamageCapture().ResistBluntDef);
	RelevantAttributesToCapture.Add(GetMortisDamageCapture().ResistSlashDef);
	RelevantAttributesToCapture.Add(GetMortisDamageCapture().ResistMagicDef);
}

static float GetCapturedOrZero(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectAttributeCaptureDefinition& Def, const FAggregatorEvaluateParameters& Params)
{
	float CapturedValue = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Def, Params, CapturedValue);
	return CapturedValue;
}

void UMortisIncomingDamageExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	// Base Damage 불러오기
	const float BaseDamage = GetCapturedOrZero(ExecutionParams, GetMortisDamageCapture().BaseDamageDef, EvaluateParameters);

	// 각 Resist 불러오기
	const float ResistPierce = GetCapturedOrZero(ExecutionParams, GetMortisDamageCapture().ResistPierceDef, EvaluateParameters);
	const float ResistBlunt = GetCapturedOrZero(ExecutionParams, GetMortisDamageCapture().ResistBluntDef, EvaluateParameters);
	const float ResistSlash = GetCapturedOrZero(ExecutionParams, GetMortisDamageCapture().ResistSlashDef, EvaluateParameters);
	const float ResistMagic = GetCapturedOrZero(ExecutionParams, GetMortisDamageCapture().ResistMagicDef, EvaluateParameters);

	// 모션 계수
	const float MotionCoef = EffectSpec.GetSetByCallerMagnitude(MortisGameplayTags::Data_AttackScale, false, 1.f);

	float Resist = 0.0f;
	if (EffectSpec.GetDynamicAssetTags().HasTagExact(MortisGameplayTags::Data_AttackType_Pierce))      Resist = ResistPierce;
	else if (EffectSpec.GetDynamicAssetTags().HasTagExact(MortisGameplayTags::Data_AttackType_Blunt))  Resist = ResistBlunt;
	else if (EffectSpec.GetDynamicAssetTags().HasTagExact(MortisGameplayTags::Data_AttackType_Slash))  Resist = ResistSlash;
	else if (EffectSpec.GetDynamicAssetTags().HasTagExact(MortisGameplayTags::Data_AttackType_Slash))  Resist = ResistMagic;

	float IncomingDamage = BaseDamage * MotionCoef * (1.0f - Resist);
	IncomingDamage = FMath::Max(0.0f, IncomingDamage);

	if (IncomingDamage > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetMortisDamageCapture().IncomingDamageProperty,
				EGameplayModOp::Override,
				IncomingDamage
			)
		);
	}
}
