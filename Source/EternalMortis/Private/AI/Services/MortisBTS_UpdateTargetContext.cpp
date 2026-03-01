// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/MortisBTS_UpdateTargetContext.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Kismet/KismetMathLibrary.h"

UMortisBTS_UpdateTargetContext::UMortisBTS_UpdateTargetContext()
{
	NodeName = TEXT("Calculate Target Distance");

	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	Interval = 0.1f;
	RandomDeviation = 0.05f;

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorKey), AActor::StaticClass());
	DistToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, DistToTargetKey));
	AngleToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, AngleToTargetKey));
}

void UMortisBTS_UpdateTargetContext::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetActorKey.ResolveSelectedKey(*BBAsset);
		DistToTargetKey.ResolveSelectedKey(*BBAsset);
		AngleToTargetKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UMortisBTS_UpdateTargetContext::GetStaticDescription() const
{
	return FString::Printf(TEXT("Update %s Context %s"),
		*TargetActorKey.SelectedKeyName.ToString(),
		*GetStaticServiceDescription()
	);
}

void UMortisBTS_UpdateTargetContext::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
		BBComp->SetValueAsFloat(DistToTargetKey.SelectedKeyName, Distance);

		FVector EnemyForward = AIC->GetPawn()->GetActorForwardVector();
		FVector EnemyToTarget = TargetActor->GetActorLocation() - AIC->GetPawn()->GetActorLocation();
		
		float Dot = FVector::DotProduct(EnemyForward, EnemyToTarget);
		float CrossZ = FVector::CrossProduct(EnemyForward, EnemyToTarget).Z;
		float AngleToTarget = FMath::RadiansToDegrees(FMath::Atan2(CrossZ, Dot));
		BBComp->SetValueAsFloat(AngleToTargetKey.SelectedKeyName, AngleToTarget);
	}
}
