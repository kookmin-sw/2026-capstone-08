// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MortisGA_HitReact.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MortisDebugHelper.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/MortisCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

void UMortisGA_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	check(ActorInfo);
	if (!TriggerEventData || !TriggerEventData->Instigator)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	const AActor* Attacker = TriggerEventData->Instigator;
	if (!OwnerActor || !Attacker)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	bFaceAttacker = bUseDirectionalReaction ? false : bFaceAttacker;
	if (bFaceAttacker)
	{
		FVector AttackerLocation = Attacker->GetActorLocation();
		FVector OwnerActorLocation = OwnerActor->GetActorLocation();
		FRotator RotToFace = UKismetMathLibrary::FindLookAtRotation(OwnerActorLocation, AttackerLocation);
		OwnerActor->SetActorRotation(RotToFace);
	}

	UAnimMontage* MontageToPlay;
	if (bUseDirectionalReaction)
	{
		FVector OwnerActorForward = OwnerActor->GetActorForwardVector();
		FVector OwnerActorToAttacker = Attacker->GetActorLocation() - OwnerActor->GetActorLocation();
		float Dot = FVector::DotProduct(OwnerActorForward, OwnerActorToAttacker);
		float CrossZ = FVector::CrossProduct(OwnerActorForward, OwnerActorToAttacker).Z;
		float AngleToTarget = FMath::RadiansToDegrees(FMath::Atan2(CrossZ, Dot));
		MontageToPlay = SelectHitReactMontage(AngleToTarget);
	}
	else
	{
		MontageToPlay = SelectRandomMontage(ForwardHitMontages);
	}
	
	if (!MontageToPlay)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("HitReact"),
		MontageToPlay,
		1.f
	);

	if (PlayMontageTask)
	{
		PlayMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
		PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCompleted);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		
		PlayMontageTask->ReadyForActivation();
	}
}

UAnimMontage* UMortisGA_HitReact::SelectHitReactMontage(float AngleToAttacker)
{
	if (AngleToAttacker >= -45.f && AngleToAttacker <= 45.f)
	{
		return SelectRandomMontage(ForwardHitMontages);
	}
	if (AngleToAttacker > 45.f && AngleToAttacker <= 135.f)
	{
		return SelectRandomMontage(RightHitMontages);
	}
	if (AngleToAttacker < -45.f && AngleToAttacker >= -135.f)
	{
		return SelectRandomMontage(LeftHitMontages);
	}
	return SelectRandomMontage(BackHitMontages);
}

void UMortisGA_HitReact::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

UAnimMontage* UMortisGA_HitReact::SelectRandomMontage(const TArray<UAnimMontage*>& Montages)
{
	int32 RandomIndex = FMath::RandRange(0, Montages.Num() - 1);
	return Montages.IsValidIndex(RandomIndex) ? Montages[RandomIndex] : nullptr;
}
