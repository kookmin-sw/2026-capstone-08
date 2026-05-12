// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/MortisEnemyAbilitySet.h"
#include "AbilitySystem/Abilities/Enemy/MortisEnemyGameplayAbility.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "Character/Enemy/MortisEnemyData.h"
#include "MortisDebugHelper.h"

#include "GameplayAbilitySpec.h"

void UMortisEnemyAbilitySet::GiveToEnemy(UMortisAbilitySystemComponent* ASC, const FMortisEnemyStats& EnemyStats, int32 ApplyLevel)
{
	if (!ASC || !StatsEffectClass)
	{
		MORTIS_LOG("ASC is null!");
		return;
	}
	
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(StatsEffectClass, 1.f, EffectContext);
	
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(MortisGameplayTags::Data_Enemy_Stat_MaxHealth, EnemyStats.MaxHealth);
		SpecHandle.Data->SetSetByCallerMagnitude(MortisGameplayTags::Data_Enemy_Stat_MaxPoise, EnemyStats.MaxPoise);
		SpecHandle.Data->SetSetByCallerMagnitude(MortisGameplayTags::Data_Enemy_Stat_BaseDamage, EnemyStats.BaseDamage);
		SpecHandle.Data->SetSetByCallerMagnitude(MortisGameplayTags::Data_Enemy_Stat_ResistPierce, EnemyStats.ResistPierce);
		SpecHandle.Data->SetSetByCallerMagnitude(MortisGameplayTags::Data_Enemy_Stat_ResistBlunt, EnemyStats.ResistBlunt);
		SpecHandle.Data->SetSetByCallerMagnitude(MortisGameplayTags::Data_Enemy_Stat_ResistSlash, EnemyStats.ResistSlash);
		SpecHandle.Data->SetSetByCallerMagnitude(MortisGameplayTags::Data_Enemy_Stat_ResistMagic, EnemyStats.ResistMagic);
		
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	
	GiveToAbilitySystemComponent(ASC, ApplyLevel);
}

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
