// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MortisGameplayAbility.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "Components/Combat/MortisCombatComponent.h"

void UMortisGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (AbilityActivationPolicy == EMortisAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
	}
}

void UMortisGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActorInfo && AbilityActivationPolicy == EMortisAbilityActivationPolicy::OnGiven)
		ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
}

UMortisCombatComponent* UMortisGameplayAbility::GetMortisCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UMortisCombatComponent>();
}
