// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/MortisEnemyGameplayAbility.h"

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
