// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/MortisGA_ExecuteAttackPattern.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/Combat/MortisEnemyCombatComponent.h"
#include "Items/Weapons/MortisEnemyWeapon.h"
#include "Items/Weapons/MortisWeaponBase.h"
#include "MortisDebugHelper.h"

#include "MortisFunctionLibrary.h"
#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/AbilityTasks/MortisAT_UpdateWarpTarget.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Spawn/MortisSpawnConfig.h"

namespace MortisAttackPatternConsts
{
	FGameplayTag HitEventTag = MortisGameplayTags::Event_Combat_AttackHit;
	FGameplayTag ComboTransitionEventTag = MortisGameplayTags::Event_Combat_Combo_Next;
	FGameplayTag DamageTag = MortisGameplayTags::Data_Enemy_Stat_WeaponDamage;
	FGameplayTag SpawnTag = MortisGameplayTags::Event_Action_Spawn;
}

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
		// MORTIS_LOG("Target Actor is null");
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
	// MORTIS_LOG("End GA_ExecuteAttackPattern");
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
				0.1f
			);
			if (CachedUpdateWarpTargetTask.IsValid())
			{
				CachedUpdateWarpTargetTask->ReadyForActivation();
			}
			
			CachedWarpStopTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				MortisGameplayTags::Event_Montage_EndWarpUpdate,
				nullptr,
				true
			);
			if (CachedWarpStopTask.IsValid())
			{
				CachedWarpStopTask->EventReceived.AddDynamic(this, &ThisClass::OnStopWarpUpdateEventReceived);
				CachedWarpStopTask->ReadyForActivation();
			}
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
			WarpTarget.Location = UMortisFunctionLibrary::CalculateWarpTargetLocation(
				GetAvatarActorFromActorInfo(),
				CachedTargetActor.Get(),
				Step.WarpTargetMode,
				Step.DesiredDistance
			);
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
		1.f
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
		MortisAttackPatternConsts::HitEventTag,
		nullptr,
		true
	);

	if (CachedWaitHitTask.IsValid())
	{
		MORTIS_LOG("Add HitEvent");
		CachedWaitHitTask->EventReceived.AddDynamic(this, &ThisClass::OnHitEventReceived);
		CachedWaitHitTask->ReadyForActivation();
	}

	if (Step.SpawnConfigClass)
	{
		CachedWaitSpawnTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			MortisAttackPatternConsts::SpawnTag,
			nullptr,
			false,
			true
		);
		if (CachedWaitSpawnTask.IsValid())
		{
			CachedWaitSpawnTask->EventReceived.AddDynamic(this, &ThisClass::OnSpawnEventReceived);
			CachedWaitSpawnTask->ReadyForActivation();
		}
	}
	
	if (Step.bUseComboTransitionNotify)
	{
		CachedComboTransitionTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			MortisAttackPatternConsts::ComboTransitionEventTag,
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
	CurrentStepIndex++;

	ResetCachedTasks();
	ExecuteNextStep();
}

void UMortisGA_ExecuteAttackPattern::OnStopWarpUpdateEventReceived(FGameplayEventData Payload)
{
	if (CachedUpdateWarpTargetTask.IsValid())
	{
		CachedUpdateWarpTargetTask->EndTask();
		CachedUpdateWarpTargetTask.Reset();
	}
	
	if (CachedWarpStopTask.IsValid())
	{
		CachedWarpStopTask->EndTask();
		CachedWarpStopTask.Reset();
	}
}

void UMortisGA_ExecuteAttackPattern::OnHitEventReceived(FGameplayEventData Payload)
{
	MORTIS_LOG("OnHitEventReceived");
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

	AMortisEnemyWeapon* Weapon = CombatComponent->GetCurrentEnemyWeapon();
	const FMortisWeaponCommonData& WeaponData = Weapon ? Weapon->GetEnemyWeaponData().CommonData : CombatComponent->GetUnarmedData();
	
	NativeApplyEffectSpecHandleToTarget(Payload.Target, MakeWeaponDamageEffectSpecHandle(WeaponData));
}

void UMortisGA_ExecuteAttackPattern::OnSpawnEventReceived(FGameplayEventData Payload)
{
	if (!AttackPattern || !AttackPattern->Steps.IsValidIndex(CurrentStepIndex) || !AttackPattern->Steps[CurrentStepIndex].SpawnConfigClass || !CachedTargetActor.IsValid())
	{
		MORTIS_LOG("");
		return;
	}
	UMortisSpawnConfig* SpawnConfig = AttackPattern->Steps[CurrentStepIndex].SpawnConfigClass->GetDefaultObject<UMortisSpawnConfig>();
	
	UMortisEnemyCombatComponent* CombatComponent = GetEnemyCombatComponent();
	if (!CombatComponent)
	{
		MORTIS_LOG("Combat Component is invalid");
		return;
	}
	
	AMortisEnemyWeapon* Weapon = CombatComponent->GetCurrentEnemyWeapon(); 
	const FMortisWeaponCommonData& WeaponData = Weapon ? Weapon->GetEnemyWeaponData().CommonData : CombatComponent->GetUnarmedData();
	const FGameplayEffectSpecHandle& SpecHandle = MakeWeaponDamageEffectSpecHandle(WeaponData);
	
	SpawnConfig->Execute(GetEnemyCharacterFromActorInfo(), CachedTargetActor.Get(), SpecHandle);
}

void UMortisGA_ExecuteAttackPattern::OnComboTransitionReceived(FGameplayEventData Payload)
{
	OnStepFinished(false);
}

FGameplayEffectSpecHandle UMortisGA_ExecuteAttackPattern::MakeWeaponDamageEffectSpecHandle(const FMortisWeaponCommonData& WeaponData)
{
	FGameplayEffectSpecHandle SpecHandle = MakeDamageEffectSpecHandle(DamageEffectClass, WeaponData, MortisAttackPatternConsts::DamageTag);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		MortisGameplayTags::Data_AttackScale,
		AttackPattern->Steps[CurrentStepIndex].DamageMultiplier
	);
	
	return SpecHandle;
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
	
	if (CachedWarpStopTask.IsValid())
	{
		CachedWarpStopTask->EndTask();
		CachedWarpStopTask.Reset();
	}
	
	if (CachedWaitHitTask.IsValid())
	{
		CachedWaitHitTask->EndTask();
		CachedWaitHitTask.Reset();
	}

	if (CachedWaitSpawnTask.IsValid())
	{
		CachedWaitSpawnTask->EndTask();
		CachedWaitSpawnTask.Reset();
	}
	
	if (CachedComboTransitionTask.IsValid())
	{
		CachedComboTransitionTask->EndTask();
		CachedComboTransitionTask.Reset();
	}
	
	if (UAnimInstance* AnimInstance = GetEnemyCharacterFromActorInfo()->GetMesh()->GetAnimInstance())
	{
		AnimInstance->StopAllMontages(0.25f);
	}
}
