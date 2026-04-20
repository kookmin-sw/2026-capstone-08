// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/MortisEnemyGameplayAbility.h"

#include "MortisDebugHelper.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Controllers/MortisAIController.h"

AMortisEnemyCharacter* UMortisEnemyGameplayAbility::GetEnemyCharacterFromActorInfo() const
{
	return Cast<AMortisEnemyCharacter>(GetAvatarActorFromActorInfo());
}

AMortisAIController* UMortisEnemyGameplayAbility::GetAIControllerFromActorInfo() const
{
	if (AMortisEnemyCharacter* EnemyCharacter = GetEnemyCharacterFromActorInfo())
	{
		return Cast<AMortisAIController>(EnemyCharacter->GetController()); 
	}
	return nullptr;
}

UMortisEnemyCombatComponent* UMortisEnemyGameplayAbility::GetEnemyCombatComponent() const
{
	if (AMortisEnemyCharacter* EnemyCharacter = GetEnemyCharacterFromActorInfo())
	{
		return EnemyCharacter->GetEnemyCombatComponent();	
	}
	return nullptr;
}


FGameplayEffectSpecHandle UMortisEnemyGameplayAbility::MakeDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FMortisWeaponCommonData& WeaponData, const FGameplayTag& DamageTag)
{
	if (!EffectClass)
	{
		MORTIS_LOG("Effect class is invalid!");
		return FGameplayEffectSpecHandle();
	}

	FGameplayEffectContextHandle ContextHandle = GetMortisAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle EffectSpecHandle = GetMortisAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);

	// MORTIS_LOG("WeaponDamage: %f, PoiseDamage: %f", WeaponData.WeaponDamage, WeaponData.PoiseDamage);
	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		DamageTag,
		WeaponData.WeaponDamage
	);
	
	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		MortisGameplayTags::Data_Stat_PoiseDamage,
		WeaponData.PoiseDamage
	);
	
	return EffectSpecHandle;
}
