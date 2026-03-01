// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/MortisBTT_SelectAttackPattern.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Components/Combat/MortisEnemyCombatComponent.h"

UMortisBTT_SelectAttackPattern::UMortisBTT_SelectAttackPattern()
{
	bNotifyTick = false;
	bNotifyTaskFinished = false;
	bCreateNodeInstance = false;
	
	TargetDistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetDistanceKey));
	DotToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, DotToTargetKey));
	AttackPatternIndexKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, AttackPatternIndexKey));
}

void UMortisBTT_SelectAttackPattern::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetDistanceKey.ResolveSelectedKey(*BBAsset);
		DotToTargetKey.ResolveSelectedKey(*BBAsset);
		AttackPatternIndexKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UMortisBTT_SelectAttackPattern::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}

EBTNodeResult::Type UMortisBTT_SelectAttackPattern::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMortisEnemyCharacter* EnemyCharacter = GetEnemyCharacter(OwnerComp);
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (EnemyCharacter && BBComp)
	{
		if (UMortisEnemyCombatComponent* CombatComp = EnemyCharacter->GetEnemyCombatComponent())
		{
			float TargetDistance = BBComp->GetValueAsFloat(TargetDistanceKey.SelectedKeyName);
			float DotToTarget = BBComp->GetValueAsFloat(DotToTargetKey.SelectedKeyName);
			int32 AttackPatternIndex = CombatComp->SelectAttackPattern(TargetDistance, DotToTarget);

			if (AttackPatternIndex != INDEX_NONE)
			{
				BBComp->SetValueAsFloat(AttackPatternIndexKey.SelectedKeyName, AttackPatternIndex);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}