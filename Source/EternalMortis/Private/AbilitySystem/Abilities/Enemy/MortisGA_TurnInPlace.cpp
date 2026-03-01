// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/MortisGA_TurnInPlace.h"
#include "MortisDebugHelper.h"

#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Kismet/KismetMathLibrary.h"


class UMotionWarpingComponent;

void UMortisGA_TurnInPlace::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !TriggerEventData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AMortisEnemyCharacter* Enemy = GetEnemyCharacterFromActorInfo();
	if (!Enemy)
	{
		MORTIS_LOG("MotionWarpingComp is null");
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	
	UAnimMontage* TurnMontage = SelectBestMontage(TriggerEventData->EventMagnitude);
	if (!TurnMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	// MORTIS_LOG("TurnMontage: %s", *TurnMontage->GetName());
	UMotionWarpingComponent* MotionWarpingComp = GetMotionWarpingComponent();
	if (!MotionWarpingComp)
	{
		MORTIS_LOG("MotionWarpingComp is null");
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (!TriggerEventData->Target)
	{
		MORTIS_LOG("Target is null");
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	
	FMotionWarpingTarget WarpTarget;
	WarpTarget.Name = WarpTargetName;
	WarpTarget.Location = TriggerEventData->Target->GetActorLocation();
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
		ActorInfo->AvatarActor->GetActorLocation(), 
		TriggerEventData->Target->GetActorLocation()
	);
	WarpTarget.Rotation = LookAtRot;
	MotionWarpingComp->AddOrUpdateWarpTarget(WarpTarget);
	
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("TurnTask"),
		TurnMontage
	);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);

	MontageTask->ReadyForActivation();
}

UAnimMontage* UMortisGA_TurnInPlace::SelectBestMontage(float AngleToTarget)
{
	// MORTIS_LOG("AngleToTarget: %f", AngleToTarget);
	const float AbsAngle = FMath::Abs(AngleToTarget);
	
	if (AbsAngle < Turn90Threshold) return nullptr;

	if (AngleToTarget >= 0.f)
	{
		return (AbsAngle >= Turn180Threshold) ? MontageTurnRight180 : MontageTurnRight90;
	}
	return (AbsAngle >= Turn180Threshold) ? MontageTurnLeft180 : MontageTurnLeft90;
}

void UMortisGA_TurnInPlace::OnMontageCompleted()
{
	MORTIS_LOG("Turn Completed");
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}