// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/MortisBTT_RotateToTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UMortisBTT_RotateToTarget::UMortisBTT_RotateToTarget()
{
	NodeName = TEXT("Rotate to Target");

	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;

	INIT_TASK_NODE_NOTIFY_FLAGS();

	TargetToFaceKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetToFaceKey), AActor::StaticClass());
}

void UMortisBTT_RotateToTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetToFaceKey.ResolveSelectedKey(*BBAsset);
	}
}

uint16 UMortisBTT_RotateToTarget::GetInstanceMemorySize() const
{
	return sizeof(FRotateToTargetTaskMemory);
}

FString UMortisBTT_RotateToTarget::GetStaticDescription() const
{
	const FString KeyDescription = TargetToFaceKey.SelectedKeyName.ToString();

	return FString::Printf(TEXT("Smoothly rotates to face %s Key until angle precision %s"), *KeyDescription, *FString::SanitizeFloat(AnglePrecision));
}

EBTNodeResult::Type UMortisBTT_RotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp || !OwnerComp.GetAIOwner())
	{
		return EBTNodeResult::Failed;
	}
	
	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetToFaceKey.SelectedKeyName));
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	FRotateToTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToTargetTaskMemory>(NodeMemory);
	if (!Memory)
	{
		return EBTNodeResult::Failed;
	}
	Memory->OwningPawn = OwningPawn;
	Memory->TargetActor = TargetActor;

	if (!Memory->IsValid())
	{
		return EBTNodeResult::Failed;
	}

	if (HasReachedAnglePrecision(OwningPawn, TargetActor))
	{
		Memory->Reset();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::InProgress;
}

void UMortisBTT_RotateToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FRotateToTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToTargetTaskMemory>(NodeMemory);

	if (!Memory->IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (HasReachedAnglePrecision(Memory->OwningPawn.Get(), Memory->TargetActor.Get()))
	{
		Memory->Reset();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Memory->OwningPawn->GetActorLocation(), Memory->TargetActor->GetActorLocation());
		const FRotator TargetRot = FMath::RInterpTo(Memory->OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterpSpeed);

		Memory->OwningPawn->SetActorRotation(TargetRot);
	}
}

bool UMortisBTT_RotateToTarget::HasReachedAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const
{
	const FVector OwnerForward = QueryPawn->GetActorForwardVector();
	const FVector OwnerToTargetNormalized = (TargetActor->GetActorLocation() - QueryPawn->GetActorLocation()).GetSafeNormal();

	const float DotResult = FVector::DotProduct(OwnerForward, OwnerToTargetNormalized);
	const float CosThreshold = FMath::Cos(FMath::DegreesToRadians(AnglePrecision));
	
	return DotResult >= CosThreshold;
}
