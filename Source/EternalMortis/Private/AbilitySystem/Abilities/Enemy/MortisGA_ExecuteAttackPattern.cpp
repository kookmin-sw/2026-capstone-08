// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/MortisGA_ExecuteAttackPattern.h"
#include "Components/Combat/MortisEnemyCombatComponent.h"
#include "MortisDebugHelper.h"

#include "AIController.h"
#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/AbilityTasks/MortisAT_UpdateWarpTarget.h"
#include "Kismet/KismetMathLibrary.h"

UMortisGA_ExecuteAttackPattern::UMortisGA_ExecuteAttackPattern()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UMortisGA_ExecuteAttackPattern::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// MORTIS_LOG("");

	if (!ActorInfo || !TriggerEventData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UMortisEnemyCombatComponent* CombatComp = GetEnemyCombatComponent();
	if (!CombatComp)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	int32 PatternIndex = static_cast<int32>(TriggerEventData->EventMagnitude);

	AttackPattern = CombatComp->GetAttackPatternByIndex(PatternIndex);
	if (!AttackPattern)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	CachedTargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
	if (!CachedTargetActor.IsValid())
	{
		MORTIS_LOG("Target Actor is null");
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CurrentStepIndex = 0;

	CachedMotionWarpingComp = GetMotionWarpingComponent();
	if (!CachedMotionWarpingComp.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	ExecuteNextStep();
}


void UMortisGA_ExecuteAttackPattern::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UpdateWarpTargetTask.IsValid())
	{
		UpdateWarpTargetTask->EndTask();
	}
	AttackPattern = nullptr;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMortisGA_ExecuteAttackPattern::ExecuteNextStep()
{
	if (!AttackPattern || !AttackPattern->Steps.IsValidIndex(CurrentStepIndex) || !CurrentActorInfo)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	const FMortisAttackPatternStep& Step = AttackPattern->Steps[CurrentStepIndex];
	if (!Step.Montage || !CachedTargetActor.IsValid())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	if (Step.bUseMotionWarping)
	{
		if (Step.bContinuousWarpUpdate)
		{
			UpdateWarpTargetTask = UMortisAT_UpdateWarpTarget::UpdateWarpTarget(
				this,
				Step.WarpTargetName,
				CachedTargetActor.Get(),
				Step.WarpTargetMode,
				Step.DesiredDistance,
				0.1f,
				Step.WarpUpdateDuration
			);
			UpdateWarpTargetTask->ReadyForActivation();
		}
		else
		{
			if (!CachedMotionWarpingComp.IsValid())
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
				return;
			}
			FMotionWarpingTarget WarpTarget;
			WarpTarget.Name = Step.WarpTargetName;
			WarpTarget.Location = CachedTargetActor->GetActorLocation();
			FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
				CurrentActorInfo->AvatarActor->GetActorLocation(), 
				CachedTargetActor->GetActorLocation()
			);
			WarpTarget.Rotation = LookAtRot;
			CachedMotionWarpingComp->AddOrUpdateWarpTarget(WarpTarget);
		}
		LastWarpTargetName = Step.WarpTargetName;
	}
	
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, 
		NAME_None, 
		Step.Montage, 
		Step.PlayRate
	);
	
	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnStepMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnStepMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnStepMontageCancelled);
		
		MontageTask->ReadyForActivation();
	}
	else
	{
		MORTIS_LOG("Invalid Montage Task!");
	}
}

void UMortisGA_ExecuteAttackPattern::OnStepMontageCompleted()
{
	OnStepFinished(false);
}

void UMortisGA_ExecuteAttackPattern::OnStepMontageInterrupted()
{
	OnStepFinished(true);
}

void UMortisGA_ExecuteAttackPattern::OnStepMontageCancelled()
{
	OnStepFinished(true);
}

void UMortisGA_ExecuteAttackPattern::OnStepFinished(bool bInterrupted)
{
	if (bInterrupted || !AttackPattern)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (CachedMotionWarpingComp.IsValid() && LastWarpTargetName != NAME_None)
	{
		CachedMotionWarpingComp->RemoveWarpTarget(LastWarpTargetName);
	}
	const FMortisAttackPatternStep& Step = AttackPattern->Steps[CurrentStepIndex];
	CurrentStepIndex++;

	
	if (UpdateWarpTargetTask.IsValid())
	{
		UpdateWarpTargetTask->EndTask();
		UpdateWarpTargetTask.Reset();
	}
	if (Step.DelayAfterStep > 0.0f)
	{
		UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, Step.DelayAfterStep);
		DelayTask->OnFinish.AddDynamic(this, &ThisClass::ExecuteNextStep);
		DelayTask->ReadyForActivation();
	}
	else
	{
		ExecuteNextStep();
	}
}
