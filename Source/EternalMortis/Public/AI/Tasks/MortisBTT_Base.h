// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MortisBTT_Base.generated.h"

class UMortisAbilitySystemComponent;
class AMortisAIController;
class AMortisEnemyCharacter;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBTT_Base : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	AMortisEnemyCharacter* GetEnemyCharacter(UBehaviorTreeComponent& OwnerComp) const;
	AMortisAIController* GetMortisAIController(UBehaviorTreeComponent& OwnerComp) const;
	UMortisAbilitySystemComponent* GetMortisASC(UBehaviorTreeComponent& OwnerComp) const;
};
