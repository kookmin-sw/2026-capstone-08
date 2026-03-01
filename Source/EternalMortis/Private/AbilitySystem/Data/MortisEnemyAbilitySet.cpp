// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/MortisEnemyAbilitySet.h"
#include "AbilitySystem/Abilities/Enemy/MortisEnemyGameplayAbility.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"

#include "GameplayAbilitySpec.h"
#include "MortisDebugHelper.h"

void UMortisEnemyAbilitySet::GiveToAbilitySystemComponent(UMortisAbilitySystemComponent* ASCToGive, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(ASCToGive, ApplyLevel);

	// MORTIS_LOG("");
	for (const TSubclassOf<UMortisEnemyGameplayAbility>& Ability : EnemyAbilities)
	{
		if (!Ability)
		{
			continue;
		}

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = ASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		ASCToGive->GiveAbility(AbilitySpec);
	}
}