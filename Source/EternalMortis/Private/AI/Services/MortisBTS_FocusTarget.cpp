// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Services/MortisBTS_FocusTarget.h"

#include "AIController.h"
#include "MortisDebugHelper.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UMortisBTS_FocusTarget::UMortisBTS_FocusTarget()
{
	NodeName = TEXT("Focus Target");
	
	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorKey), AActor::StaticClass());
}

void UMortisBTS_FocusTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UMortisBTS_FocusTarget::GetStaticDescription() const
{
	return FString::Printf(TEXT("Focus %s %s"), *TargetActorKey.SelectedKeyName.ToString(), *GetStaticServiceDescription());
}

void UMortisBTS_FocusTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BBComp)
	{
		return;
	}

	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		MORTIS_LOG("Target Actor is null");
		return;
	}

	// MORTIS_LOG("SetFocus: %s", *TargetActor->GetActorNameOrLabel());
	AIC->SetFocus(TargetActor, EAIFocusPriority::Gameplay);
}

void UMortisBTS_FocusTarget::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return;
	}
	// MORTIS_LOG("Clear Focus");
	AIC->ClearFocus(EAIFocusPriority::Gameplay);
}
