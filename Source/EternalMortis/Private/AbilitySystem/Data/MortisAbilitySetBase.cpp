// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/MortisAbilitySetBase.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/MortisGameplayAbility.h"

void UMortisAbilitySetBase::GiveToAbilitySystemComponent(UMortisAbilitySystemComponent* ASCToGive, int32 ApplyLevel)
{
	check(ASCToGive);

	GrantAbilities(ActivateOnGivenAbilities, ASCToGive, ApplyLevel);
	GrantAbilities(ReactiveAbilities, ASCToGive, ApplyLevel);
}

void UMortisAbilitySetBase::GrantAbilities(const TArray<TSubclassOf<UMortisGameplayAbility>>& AbilitySet, UMortisAbilitySystemComponent* ASCToGive, int32 ApplyLevel)
{
	if (AbilitySet.IsEmpty()) return;

	for (const TSubclassOf<UMortisGameplayAbility>& Ability : AbilitySet)
	{
		if (!Ability) continue;

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = ASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		ASCToGive->GiveAbility(AbilitySpec);
	}
}
