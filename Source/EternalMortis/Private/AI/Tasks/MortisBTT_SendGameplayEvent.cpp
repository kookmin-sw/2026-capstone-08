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
	
	INIT_TASK_NODE_NOTIFY_FLAGS();

	EventReceiverKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, EventReceiverKey), AActor::StaticClass());
	PayloadTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, PayloadTargetKey), AActor::StaticClass());
	EventMagnitude.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, EventMagnitude));
}

void UMortisBTT_SendGameplayEvent::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		EventReceiverKey.ResolveSelectedKey(*BBAsset);
		PayloadTargetKey.ResolveSelectedKey(*BBAsset);
		EventMagnitude.ResolveSelectedKey(*BBAsset);
	}
}

uint16 UMortisBTT_SendGameplayEvent::GetInstanceMemorySize() const
{
	return sizeof(FBTSendGameplayEventTaskMemory);
}

EBTNodeResult::Type UMortisBTT_SendGameplayEvent::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp || !EventTag.IsValid())
	{
		MORTIS_LOG("BBComp or EventTag is invalid");
		return EBTNodeResult::Failed;
	}
	
	AActor* ReceiverActor = Cast<AActor>(BBComp->GetValueAsObject(EventReceiverKey.SelectedKeyName));
	if (!ReceiverActor)
	{
		MORTIS_LOG("ReceiverActor is null");
		return EBTNodeResult::Failed;
	}

	FBTSendGameplayEventTaskMemory* Memory = CastInstanceNodeMemory<FBTSendGameplayEventTaskMemory>(NodeMemory);
	Memory->Reset();
	if (!Memory)
	{
		MORTIS_LOG("memory failed");
		return EBTNodeResult::Failed;
	}
	FGameplayEventData EventData;
	EventData.EventTag = EventTag;
	EventData.Instigator = GetEnemyCharacter(OwnerComp);
	EventData.Target = Cast<AActor>(BBComp->GetValueAsObject(PayloadTargetKey.SelectedKeyName));
	EventData.EventMagnitude = BBComp->GetValueAsFloat(EventMagnitude.SelectedKeyName); 

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(ReceiverActor, EventTag, EventData);

	if (!StateTagToWait.IsValid())
	{
		MORTIS_LOG("state tag to wait is invalid");
		return EBTNodeResult::Succeeded;
	}
	
	Memory->CachedReceiver = ReceiverActor;
	Memory->CachedASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ReceiverActor);
	
	return EBTNodeResult::InProgress;
}

void UMortisBTT_SendGameplayEvent::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	FBTSendGameplayEventTaskMemory* Memory = CastInstanceNodeMemory<FBTSendGameplayEventTaskMemory>(NodeMemory);
	if (!Memory || !Memory->IsValid())
	{
		MORTIS_LOG("Memory is invalid");
		Memory->Reset();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	if (!Memory->CachedASC->HasMatchingGameplayTag(StateTagToWait))
	{
		Memory->Reset();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
