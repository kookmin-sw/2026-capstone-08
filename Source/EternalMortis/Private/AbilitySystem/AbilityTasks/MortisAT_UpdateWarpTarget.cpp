// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/MortisAT_UpdateWarpTarget.h"

#include "MortisDebugHelper.h"
#include "MotionWarpingComponent.h"
#include "Character/MortisCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

UMortisAT_UpdateWarpTarget* UMortisAT_UpdateWarpTarget::UpdateWarpTarget(UGameplayAbility* OwningAbility,
	FName WarpTargetName, AActor* TargetActor, float UpdateInterval, float WarpUpdateDuration)
{
	UMortisAT_UpdateWarpTarget* Task = NewAbilityTask<UMortisAT_UpdateWarpTarget>(OwningAbility);
	Task->WarpTargetName = WarpTargetName;
	Task->UpdateInterval = UpdateInterval;
	Task->CachedTarget = TargetActor;
	Task->WarpUpdateDuration = WarpUpdateDuration;
	Task->bTickingTask = true;
	
	return Task;
}

void UMortisAT_UpdateWarpTarget::Activate()
{
	Super::Activate();
	
	if (!CachedTarget.IsValid())
	{
		MORTIS_LOG("Target is invalid.");
		EndTask();
		return;
	}
	AMortisCharacterBase* Character = Cast<AMortisCharacterBase>(GetAvatarActor());
	if (!Character)
	{
		MORTIS_LOG("Character is null");
		EndTask();
		return;
	}
	CachedMWC = Character->GetMotionWarpingComponent();

	if (!CachedMWC.IsValid())
	{
		MORTIS_LOG("MotionWarpingComponent is invalid.");
		EndTask();
		return;
	}
	
	UpdateTarget();
}


void UMortisAT_UpdateWarpTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (WarpUpdateDuration > 0.f)
	{
		TotalElapsedTime += DeltaTime;
		if (TotalElapsedTime >= WarpUpdateDuration)
		{
			EndTask();
			return;
		}
	}
	
	TimeSinceLastUpdate += DeltaTime;
	if (TimeSinceLastUpdate > UpdateInterval)
	{
		TimeSinceLastUpdate = 0.f;
		UpdateTarget();
	}
}

// void UMortisAT_UpdateWarpTarget::OnDestroy(bool bInOwnerFinished)
// {
// 	if (CachedMWC.IsValid())
// 	{
// 		CachedMWC->RemoveWarpTarget(WarpTargetName);
// 	}
// 	
// 	Super::OnDestroy(bInOwnerFinished);
// }


void UMortisAT_UpdateWarpTarget::UpdateTarget()
{
	if (!CachedTarget.IsValid())
	{
		MORTIS_LOG("Target Actor is invalid. Ending task");
		EndTask();
		return;
	}
	FMotionWarpingTarget WarpTarget;
	WarpTarget.Name = WarpTargetName;
	WarpTarget.Location = CachedTarget->GetActorLocation();
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
		GetAvatarActor()->GetActorLocation(), 
		CachedTarget->GetActorLocation()
	);
	WarpTarget.Rotation = LookAtRot;
	CachedMWC->AddOrUpdateWarpTarget(WarpTarget);

	// MORTIS_LOG("Update: %f, %f", CachedTarget->GetActorLocation().X, CachedTarget->GetActorLocation().Y);
}
