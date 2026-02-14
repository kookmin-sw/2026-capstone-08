// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/MortisBTS_CalculateDistance.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"

UMortisBTS_CalculateDistance::UMortisBTS_CalculateDistance()
{
	NodeName = TEXT("Calculate Target Distance");

	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	Interval = 0.1f;
	RandomDeviation = 0.05f;

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorKey), AActor::StaticClass());
	DistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, DistanceKey));
}

void UMortisBTS_CalculateDistance::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UMortisBTS_CalculateDistance::GetStaticDescription() const
{
	return FString::Printf(TEXT("Calculate distance to %s Key %s"),
		*TargetActorKey.SelectedKeyName.ToString(),
		*GetStaticServiceDescription()
	);
}

void UMortisBTS_CalculateDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AAIController* AIC = OwnerComp.GetAIOwner();

	if (!BBComp || !AIC || !AIC->GetPawn())
	{
		return;
	}
	
	const AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (TargetActor)
	{
		float Distance = FVector::Dist(AIC->GetPawn()->GetActorLocation(), TargetActor->GetActorLocation());
		BBComp->SetValueAsFloat(DistanceKey.SelectedKeyName, Distance);
	}
}
