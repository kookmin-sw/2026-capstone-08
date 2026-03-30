// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/MortisEnemyGameplayAbility.h"

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


FGameplayEffectSpecHandle UMortisEnemyGameplayAbility::MakeDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float WeaponDamage, const FGameplayTag& DamageTag)
{
	if (!EffectClass)
	{
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

	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		DamageTag,
		WeaponDamage
	);
	
	return EffectSpecHandle;
}
