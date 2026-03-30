// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/MortisGA_ExecuteAttackPattern.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/Combat/MortisEnemyCombatComponent.h"
#include "MortisDebugHelper.h"

#include "AIController.h"
#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/AbilityTasks/MortisAT_UpdateWarpTarget.h"
#include "Items/Weapons/MortisEnemyWeapon.h"
#include "Items/Weapons/MortisWeaponBase.h"
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
	ResetCachedTasks();
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
		MORTIS_LOG("Montage or CachedTarget is invalid");
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	if (Step.bUseMotionWarping)
	{
		if (Step.bContinuousWarpUpdate)
		{
			CachedUpdateWarpTargetTask = UMortisAT_UpdateWarpTarget::UpdateWarpTarget(
				this,
				Step.WarpTargetName,
				CachedTargetActor.Get(),
				Step.WarpTargetMode,
				Step.DesiredDistance,
				0.1f,
				Step.WarpUpdateDuration
			);
			CachedUpdateWarpTargetTask->ReadyForActivation();
		}
		else
		{
			if (!CachedMotionWarpingComp.IsValid())
			{
				MORTIS_LOG("Warp Target is invalid");
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
	
	CachedMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, 
		NAME_None, 
		Step.Montage, 
		Step.PlayRate
	);
	
	if (CachedMontageTask.IsValid())
	{
		CachedMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnStepMontageCompleted);
		CachedMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnStepMontageInterrupted);
		CachedMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnStepMontageCancelled);
		
		CachedMontageTask->ReadyForActivation();
	}
	else
	{
		MORTIS_LOG("Invalid Montage Task!");
	}

	CachedWaitHitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		HitEventTag,
		nullptr,
		false,
		true
	);

	if (CachedWaitHitTask.IsValid())
	{
		CachedWaitHitTask->EventReceived.AddDynamic(this, &ThisClass::OnHitEventReceived);
		CachedWaitHitTask->ReadyForActivation();
	}

	if (Step.bUseComboTransitionNotify)
	{
		CachedComboTransitionTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			ComboTransitionEventTag,
			nullptr,
			false,
			true
		);

		if (CachedComboTransitionTask.IsValid())
		{
			CachedComboTransitionTask->EventReceived.AddDynamic(this, &ThisClass::OnComboTransitionReceived);
			CachedComboTransitionTask->ReadyForActivation();
		}
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

	ResetCachedTasks();
	
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

void UMortisGA_ExecuteAttackPattern::OnHitEventReceived(FGameplayEventData Payload)
{
	if (!AttackPattern || !AttackPattern->Steps.IsValidIndex(CurrentStepIndex))
	{
		MORTIS_LOG("Attack Pattern or Step is invalid");
		return;
	}
	UMortisEnemyCombatComponent* CombatComponent = GetEnemyCombatComponent();
	if (!CombatComponent)
	{
		MORTIS_LOG("Combat Component is invalid");
		return;
	}

	AMortisEnemyWeapon* Weapon = CombatComponent->GetEnemyWeapon();
	if (!Weapon)
	{
		MORTIS_LOG("Weapon is invalid");
		return;
	}
	
	FGameplayEffectSpecHandle SpecHandle = MakeDamageEffectSpecHandle(DamageEffectClass, Weapon->GetEnemyWeaponData().WeaponDamage, DamageTag);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		MortisGameplayTags::Data_AttackScale,
		AttackPattern->Steps[CurrentStepIndex].DamageMultiplier
	);
	
	NativeApplyEffectSpecHandleToTarget(Payload.Target, SpecHandle);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(const_cast<AActor*>(Payload.Target.Get()), MortisGameplayTags::Event_Action_HitReact, Payload);
}

void UMortisGA_ExecuteAttackPattern::OnComboTransitionReceived(FGameplayEventData Payload)
{
	OnStepFinished(false);
}

void UMortisGA_ExecuteAttackPattern::ResetCachedTasks()
{
	if (CachedMontageTask.IsValid())
	{
		CachedMontageTask->OnInterrupted.Clear();
		CachedMontageTask->EndTask();
		CachedMontageTask.Reset();
	}
	
	if (CachedUpdateWarpTargetTask.IsValid())
	{
		CachedUpdateWarpTargetTask->EndTask();
		CachedUpdateWarpTargetTask.Reset();
	}
	
	if (CachedWaitHitTask.IsValid())
	{
		CachedWaitHitTask->EndTask();
		CachedWaitHitTask.Reset();
	}

	if (CachedComboTransitionTask.IsValid())
	{
		CachedComboTransitionTask->EndTask();
		CachedComboTransitionTask.Reset();
	}
}
