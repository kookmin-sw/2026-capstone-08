// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/MortisEnemyGameplayAbility.h"
#include "Types/MortisStructTypes.h"
#include "MortisGA_ExecuteAttackPattern.generated.h"

class UMortisAT_UpdateWarpTarget;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisGA_ExecuteAttackPattern : public UMortisEnemyGameplayAbility
{
	GENERATED_BODY()

public:
	UMortisGA_ExecuteAttackPattern();

protected:
	//~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface

protected:
	UFUNCTION()
	void ExecuteNextStep();

	UFUNCTION()
	void OnStepMontageCompleted();

	UFUNCTION()
	void OnStepMontageInterrupted();

	UFUNCTION()
	void OnStepMontageCancelled();
	
	UFUNCTION(BlueprintCallable)
	void OnStepFinished(bool bInterrupted);
	
private:
	UPROPERTY(VisibleAnywhere)
	int32 CurrentStepIndex = 0;

	const FMortisAttackPattern* AttackPattern;
	
	TWeakObjectPtr<UMotionWarpingComponent> CachedMotionWarpingComp;
	TWeakObjectPtr<AActor> CachedTargetActor; 
	TWeakObjectPtr<UMortisAT_UpdateWarpTarget> UpdateWarpTargetTask;

	FName LastWarpTargetName = NAME_None;
};
