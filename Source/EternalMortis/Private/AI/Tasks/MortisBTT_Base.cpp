// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/MortisBTT_Base.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Controllers/MortisAIController.h"

#include "AIController.h"

AMortisEnemyCharacter* UMortisBTT_Base::GetEnemyCharacter(UBehaviorTreeComponent& OwnerComp) const
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	return AIC ? Cast<AMortisEnemyCharacter>(AIC->GetPawn()) : nullptr;
}

AMortisAIController* UMortisBTT_Base::GetMortisAIController(UBehaviorTreeComponent& OwnerComp) const
{
	return Cast<AMortisAIController>(OwnerComp.GetAIOwner());
}

UMortisAbilitySystemComponent* UMortisBTT_Base::GetMortisASC(UBehaviorTreeComponent& OwnerComp) const
{
	AMortisEnemyCharacter* Enemy = GetEnemyCharacter(OwnerComp);
	return Enemy ? Enemy->GetMortisAbilitySystemComponent() : nullptr;
}
