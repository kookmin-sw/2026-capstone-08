// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Player/MortisPlayerGameplayAbility.h"
#include "Character/Player/MortisPlayerCharacter.h"
#include "Controllers/MortisPlayerController.h"
#include "Components/Combat/MortisPlayerCombatComponent.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MortisPlayerAttributeSet.h"
#include "MortisGameplayTags.h"


AMortisPlayerCharacter* UMortisPlayerGameplayAbility::GetMortisPlayerCharacterFromActorInfo()
{
	if (!CachedMortisPlayerCharacter.IsValid())
		CachedMortisPlayerCharacter = Cast<AMortisPlayerCharacter>(CurrentActorInfo->AvatarActor);
	return CachedMortisPlayerCharacter.IsValid() ? CachedMortisPlayerCharacter.Get() : nullptr;
}

AMortisPlayerController* UMortisPlayerGameplayAbility::GetMortisPlayerControllerFromActorInfo()
{
	if (!CachedMortisPlayerController.IsValid())
		CachedMortisPlayerController = Cast<AMortisPlayerController>(CurrentActorInfo->PlayerController);
	return CachedMortisPlayerController.IsValid() ? CachedMortisPlayerController.Get() : nullptr;
}

UMortisPlayerCombatComponent* UMortisPlayerGameplayAbility::GetMortisPlayerCombatComponent()
{
	return GetMortisPlayerCharacterFromActorInfo()->GetMortisPlayerCombatComponent();
}

FGameplayEffectSpecHandle UMortisPlayerGameplayAbility::MakePlayerBaseDamageUpdateEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float WeaponDamage, float StrCoef, float DexCoef, float IntCoef, FGameplayTag AttackType)
{
	check(EffectClass);

	FGameplayEffectContextHandle ContextHandle = GetMortisAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle EffectSpecHandle = GetMortisAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);

	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		MortisGameplayTags::Data_Player_Stat_WeaponDamage,
		WeaponDamage
	);

	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		MortisGameplayTags::Data_Player_Stat_Coefficient_Strength,
		StrCoef
	);

	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		MortisGameplayTags::Data_Player_Stat_Coefficient_Dexterity,
		DexCoef
	);

	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		MortisGameplayTags::Data_Player_Stat_Coefficient_Intelligence,
		IntCoef
	);

	if (!AttackType.IsValid()) AttackType = MortisGameplayTags::Data_AttackType_Slash;
	EffectSpecHandle.Data->AddDynamicAssetTag(AttackType);

	return EffectSpecHandle;
}

float UMortisPlayerGameplayAbility::CalculateStaminaCost(float BaseCost, float AdditionalReduceRate) const
{
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return BaseCost;

	const float ReductionRate = ASC->GetNumericAttribute(UMortisPlayerAttributeSet::GetStaminaReductionRateAttribute()) + AdditionalReduceRate;

	return -BaseCost * (1.0f - ReductionRate);
}

bool UMortisPlayerGameplayAbility::CheckStaminaCost(float BaseCost, float& OutFinalCost, float AdditionalReduceRate) const
{
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		OutFinalCost = BaseCost;
		return false;
	}

	OutFinalCost = CalculateStaminaCost(BaseCost);

	const float CurrentStamina =
		ASC->GetNumericAttribute(UMortisPlayerAttributeSet::GetCurrentStaminaAttribute()) + AdditionalReduceRate;

	return CurrentStamina >= -OutFinalCost;
}

float UMortisPlayerGameplayAbility::CalculateManaCost(float BaseCost, float AdditionalReduceRate) const
{
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return BaseCost;

	const float ReductionRate = ASC->GetNumericAttribute(UMortisPlayerAttributeSet::GetManaReductionRateAttribute()) + AdditionalReduceRate;

	return -BaseCost * (1.0f - ReductionRate);
}

bool UMortisPlayerGameplayAbility::CheckManaCost(float BaseCost, float& OutFinalCost, float AdditionalReduceRate) const
{
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		OutFinalCost = BaseCost;
		return false;
	}

	OutFinalCost = CalculateStaminaCost(BaseCost);

	const float CurrentMana =
		ASC->GetNumericAttribute(UMortisPlayerAttributeSet::GetCurrentManaAttribute()) + AdditionalReduceRate;

	return CurrentMana >= -OutFinalCost;
}
