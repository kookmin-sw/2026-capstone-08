// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/MortisEnemyGameplayAbility.h"

#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Controllers/MortisAIController.h"

AMortisEnemyCharacter* UMortisEnemyGameplayAbility::GetEnemyCharacterFromActorInfo() const
{
	return CurrentActorInfo ? Cast<AMortisEnemyCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}

AMortisAIController* UMortisEnemyGameplayAbility::GetAIControllerFromActorInfo() const
{
	if (!CurrentActorInfo)
	{
		AActor* OwningActor = CurrentActorInfo->OwnerActor.Get();
		if (APawn* OwningPawn = Cast<APawn>(OwningActor))
		{
			return Cast<AMortisAIController>(OwningPawn->GetController());
		}
	}
	return nullptr;
}
