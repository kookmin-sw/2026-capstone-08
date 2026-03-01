// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/MortisBTT_SendGameplayEvent.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "MortisDebugHelper.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

UMortisBTT_SendGameplayEvent::UMortisBTT_SendGameplayEvent()
{
	NodeName = TEXT("Send Gameplay Event");

	bNotifyTick = true;
	
	INIT_TASK_NODE_NOTIFY_FLAGS();
	
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorKey), AActor::StaticClass());
	EventMagnitude.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, EventMagnitude));
}

void UMortisBTT_SendGameplayEvent::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetActorKey.ResolveSelectedKey(*BBAsset);
		EventMagnitude.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UMortisBTT_SendGameplayEvent::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMortisEnemyCharacter* EnemyCharacter = GetEnemyCharacter(OwnerComp);
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (EnemyCharacter && BBComp && EventTag.IsValid())
	{
		FGameplayEventData EventData;
		EventData.EventTag = EventTag;
		EventData.Instigator = EnemyCharacter;
		EventData.Target = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
		EventData.EventMagnitude = BBComp->GetValueAsFloat(EventMagnitude.SelectedKeyName); 

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(EnemyCharacter, EventTag, EventData);

		if (!StateTagToWait.IsValid())
		{
			MORTIS_LOG("Succeeded");
			return EBTNodeResult::Succeeded;
		}
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

void UMortisBTT_SendGameplayEvent::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	UMortisAbilitySystemComponent* ASC = GetMortisASC(OwnerComp);
	if (ASC && !ASC->HasMatchingGameplayTag(StateTagToWait))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
