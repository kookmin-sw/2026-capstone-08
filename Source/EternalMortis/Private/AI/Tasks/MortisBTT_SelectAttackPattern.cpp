// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/MortisBTT_SelectAttackPattern.h"

#include "MortisDebugHelper.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Components/Combat/MortisEnemyCombatComponent.h"

UMortisBTT_SelectAttackPattern::UMortisBTT_SelectAttackPattern()
{
	bNotifyTick = false;
	bNotifyTaskFinished = false;
	bCreateNodeInstance = false;
	
	DistToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, DistToTargetKey));
	AngleToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, AngleToTargetKey));
	AttackPatternIndexKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, AttackPatternIndexKey));
}

void UMortisBTT_SelectAttackPattern::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		DistToTargetKey.ResolveSelectedKey(*BBAsset);
		AngleToTargetKey.ResolveSelectedKey(*BBAsset);
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
			float DistToTarget = BBComp->GetValueAsFloat(DistToTargetKey.SelectedKeyName);
			float AngleToTarget = BBComp->GetValueAsFloat(AngleToTargetKey.SelectedKeyName);
			int32 AttackPatternIndex = CombatComp->SelectAttackPattern(DistToTarget, AngleToTarget);

			if (AttackPatternIndex != INDEX_NONE)
			{
				MORTIS_LOG("AttackPatternIndex: %d", AttackPatternIndex);
				BBComp->SetValueAsFloat(AttackPatternIndexKey.SelectedKeyName, AttackPatternIndex);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}