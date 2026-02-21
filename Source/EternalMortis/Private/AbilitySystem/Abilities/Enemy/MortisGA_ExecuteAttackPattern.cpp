// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/MortisGA_ExecuteAttackPattern.h"

#include "MortisDebugHelper.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/Combat/MortisEnemyCombatComponent.h"

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
	if (UMortisEnemyCombatComponent* CombatComp = Cast<UMortisEnemyCombatComponent>(GetMortisCombatComponentFromActorInfo()))
	{
		if (!TriggerEventData)
		{
			MORTIS_LOG("TriggerEventData is null!");
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		int32 PatternIndex = static_cast<int32>(TriggerEventData->EventMagnitude);
		if (const FMortisAttackPattern* Pattern = CombatComp->GetAttackPatternByIndex(PatternIndex))
		{
			AttackPattern = *Pattern;
		}
		// MORTIS_LOG("PatternIndex = %d", PatternIndex);

		CurrentStepIndex = 0;

		ExecuteNextStep();
	}
}

void UMortisGA_ExecuteAttackPattern::ExecuteNextStep()
{
	// MORTIS_LOG("");
	if (!AttackPattern.Steps.IsValidIndex(CurrentStepIndex))
	{
		MORTIS_LOG("Current Step Index is invalid");
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	// MORTIS_LOG("Current Step Index is %d", CurrentStepIndex);

	const FMortisAttackPatternStep& Step = AttackPattern.Steps[CurrentStepIndex];

	if (!Step.Montage)
	{
		MORTIS_LOG("Montage is null");
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
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnStepMontageInterrupted);
		
		MontageTask->ReadyForActivation();
	}
	else
	{
		MORTIS_LOG("Invalid Montage Task!");
	}
}

void UMortisGA_ExecuteAttackPattern::OnStepMontageCompleted()
{
	// MORTIS_LOG("");
	OnStepFinished(false);
}

void UMortisGA_ExecuteAttackPattern::OnStepMontageInterrupted()
{
	// MORTIS_LOG("");
	OnStepFinished(true);
}

void UMortisGA_ExecuteAttackPattern::OnStepFinished(bool bInterrupted)
{
	if (bInterrupted)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo,
				   CurrentActivationInfo, true, true);
		return;
	}

	const FMortisAttackPatternStep& Step = AttackPattern.Steps[CurrentStepIndex];
	CurrentStepIndex++;

	if (Step.DelayAfterStep > 0.0f)
	{
		// MORTIS_LOG("");
		FTimerHandle DelayHandle;
		GetWorld()->GetTimerManager().SetTimer(
			DelayHandle,
			this,
			&ThisClass::ExecuteNextStep,
			Step.DelayAfterStep,
			false);
	}
	else
	{
		ExecuteNextStep();
	}
}
