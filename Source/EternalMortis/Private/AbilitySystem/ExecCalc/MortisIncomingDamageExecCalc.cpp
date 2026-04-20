// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/MortisIncomingDamageExecCalc.h"

#include "MortisDebugHelper.h"
#include "AbilitySystem/Attributes/MortisEnemyAttributeSet.h"
#include "AbilitySystem/Attributes/MortisPlayerAttributeSet.h"
#include "MortisGameplayTags.h"

struct FMortisDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(AllTypeBonus)
	DECLARE_ATTRIBUTE_CAPTUREDEF(PierceBonus)
	DECLARE_ATTRIBUTE_CAPTUREDEF(BluntBonus)
	DECLARE_ATTRIBUTE_CAPTUREDEF(SlashBonus)
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicBonus)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistPierce)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistBlunt)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistSlash)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistMagic)
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingPoiseDamage)

	FMortisDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisAttributeSet, BaseDamage, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisAttributeSet, IncomingDamage, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisAttributeSet, IncomingPoiseDamage, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisPlayerAttributeSet, AllTypeBonus, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisPlayerAttributeSet, PierceBonus, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisPlayerAttributeSet, BluntBonus, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisPlayerAttributeSet, SlashBonus, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMortisPlayerAttributeSet, MagicBonus, Source, false)
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
	// RelevantAttributesToCapture.Add(GetMortisDamageCapture().IncomingDamageDef);
	// RelevantAttributesToCapture.Add(GetMortisDamageCapture().IncomingPoiseDamageDef);
	RelevantAttributesToCapture.Add(GetMortisDamageCapture().AllTypeBonusDef);
	RelevantAttributesToCapture.Add(GetMortisDamageCapture().PierceBonusDef);
	RelevantAttributesToCapture.Add(GetMortisDamageCapture().BluntBonusDef);
	RelevantAttributesToCapture.Add(GetMortisDamageCapture().SlashBonusDef);
	RelevantAttributesToCapture.Add(GetMortisDamageCapture().MagicBonusDef);
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

	// 보너스 공격력 불러오기
	const float AllTypeBonusDamage = GetCapturedOrZero(ExecutionParams, GetMortisDamageCapture().AllTypeBonusDef, EvaluateParameters);
	const float PierceBonus = GetCapturedOrZero(ExecutionParams, GetMortisDamageCapture().PierceBonusDef, EvaluateParameters);
	const float BluntBonus = GetCapturedOrZero(ExecutionParams, GetMortisDamageCapture().BluntBonusDef, EvaluateParameters);
	const float SlashBonus = GetCapturedOrZero(ExecutionParams, GetMortisDamageCapture().SlashBonusDef, EvaluateParameters);
	const float MagicBonus = GetCapturedOrZero(ExecutionParams, GetMortisDamageCapture().MagicBonusDef, EvaluateParameters);

	// 저항과 보너스 공격력 선택
	float Resist = 0.0f;
	float Bonus = 1.0f;
	Bonus += AllTypeBonusDamage;
	if (EffectSpec.GetDynamicAssetTags().HasTagExact(MortisGameplayTags::Data_AttackType_Pierce)) { Resist = ResistPierce; Bonus += PierceBonus; }
	else if (EffectSpec.GetDynamicAssetTags().HasTagExact(MortisGameplayTags::Data_AttackType_Blunt)) { Resist = ResistBlunt; Bonus += BluntBonus; }
	else if (EffectSpec.GetDynamicAssetTags().HasTagExact(MortisGameplayTags::Data_AttackType_Slash)) { Resist = ResistSlash; Bonus += SlashBonus; }
	else if (EffectSpec.GetDynamicAssetTags().HasTagExact(MortisGameplayTags::Data_AttackType_Magic)) { Resist = ResistMagic; Bonus += MagicBonus; }

	// 모션 계수
	const float MotionCoef = EffectSpec.GetSetByCallerMagnitude(MortisGameplayTags::Data_AttackScale, false, 1.f);

	float IncomingDamage = BaseDamage * MotionCoef * Bonus * (1.0f - Resist);
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
	
	const float PoiseDamage = EffectSpec.GetSetByCallerMagnitude(MortisGameplayTags::Data_Stat_PoiseDamage, false, 1.f) * MotionCoef;
	if (PoiseDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetMortisDamageCapture().IncomingPoiseDamageProperty,
				EGameplayModOp::Override,
				PoiseDamage
			)	
		);
	}
}
