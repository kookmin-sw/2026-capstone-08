// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MortisGameplayAbility.h"
#include "MortisEnemyGameplayAbility.generated.h"

class UMortisEnemyCombatComponent;
class AMortisAIController;
class AMortisEnemyCharacter;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisEnemyGameplayAbility : public UMortisGameplayAbility
{
	GENERATED_BODY()

public:
	AMortisEnemyCharacter* GetEnemyCharacterFromActorInfo() const;
	AMortisAIController* GetAIControllerFromActorInfo() const;
	UMortisEnemyCombatComponent* GetEnemyCombatComponent() const;
};
