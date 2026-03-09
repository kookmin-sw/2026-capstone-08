// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Player/MortisPlayerGameplayAbility.h"
#include "Character/Player/MortisPlayerCharacter.h"
#include "Controllers/MortisPlayerController.h"
#include "Components/Combat/MortisPlayerCombatComponent.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
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

FGameplayEffectSpecHandle UMortisPlayerGameplayAbility::MakePlayerBaseDamageUpdateEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float WeaponDamage, float StrCoef, float DexCoef, float IntCoef)
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

	return EffectSpecHandle;
}
