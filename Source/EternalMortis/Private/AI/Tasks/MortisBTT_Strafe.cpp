// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/MortisBTT_Strafe.h"

#include "MortisDebugHelper.h"
#include "MortisFunctionLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controllers/MortisAIController.h"

UMortisBTT_Strafe::UMortisBTT_Strafe()
{
	INIT_TASK_NODE_NOTIFY_FLAGS();

	StrafingLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, StrafingLocationKey));
}

void UMortisBTT_Strafe::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = Asset.GetBlackboardAsset())
	{
		StrafingLocationKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UMortisBTT_Strafe::GetStaticDescription() const
{
	return FString::Printf(TEXT("Strafe to: %s\nInterval: %.1fs"), 
		*StrafingLocationKey.SelectedKeyName.ToString(), UpdateInterval);
}

uint16 UMortisBTT_Strafe::GetInstanceMemorySize() const
{
	return sizeof(FBTStrafeMemory);
}

EBTNodeResult::Type UMortisBTT_Strafe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{	
	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BBComp)
	{
		return EBTNodeResult::Failed;
	}

	FBTStrafeMemory* Memory = CastInstanceNodeMemory<FBTStrafeMemory>(NodeMemory);
	if (!Memory)
	{
		return EBTNodeResult::Failed;
	}
	
	FVector StrafingLoc = BBComp->GetValueAsVector(StrafingLocationKey.SelectedKeyName);
	AIC->MoveToLocation(StrafingLoc);

	Memory->LastStrafingLocation = StrafingLoc;
	Memory->ElapsedTime = 0.f;
	
	return EBTNodeResult::InProgress;
}

void UMortisBTT_Strafe::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTStrafeMemory* Memory = CastInstanceNodeMemory<FBTStrafeMemory>(NodeMemory);
	if (!Memory)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	Memory->ElapsedTime += DeltaSeconds;
	
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC || !AIC->GetPawn())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	if (FVector::DistSquared(Memory->LastStrafingLocation, AIC->GetPawn()->GetActorLocation()) < FMath::Square(AcceptableRadius)
		|| Memory->ElapsedTime >= UpdateInterval)
	{
		// bool bFinish = FVector::DistSquared(Memory->LastStrafingLocation, AIC->GetPawn()->GetActorLocation()) < FMath::Square(AcceptableRadius);
		UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
		if (!BBComp)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}
		
		FVector StrafingLoc = BBComp->GetValueAsVector(StrafingLocationKey.SelectedKeyName);
		// MORTIS_LOG("Move To: X = %f, Y = %f", StrafingLoc.X, StrafingLoc.Y);
		AIC->MoveToLocation(StrafingLoc);
		Memory->LastStrafingLocation = StrafingLoc;
		Memory->ElapsedTime = 0.f;
	}
}


EBTNodeResult::Type UMortisBTT_Strafe::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// if (AAIController* AIC = OwnerComp.GetAIOwner())
	// {
	// 	AIC->StopMovement();
	// }
	return Super::AbortTask(OwnerComp, NodeMemory);
}
