// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/MortisPlayerAbilitySet.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/MortisPlayerGameplayAbility.h"

void UMortisPlayerAbilitySet::GiveToAbilitySystemComponent(UMortisAbilitySystemComponent* ASCToGive, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(ASCToGive, ApplyLevel);

	for (const FMortisAbilityInputBinding& InputAbilitySet : PlayerInputAbilities)
	{
		if (!InputAbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(InputAbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = ASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(InputAbilitySet.InputTag);

		ASCToGive->GiveAbility(AbilitySpec);
	}
}
