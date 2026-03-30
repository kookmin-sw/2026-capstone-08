// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/MortisEnemyGameplayAbility.h"
#include "Types/MortisStructTypes.h"
#include "MortisGA_ExecuteAttackPattern.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
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
	UPROPERTY(EditDefaultsOnly, Category = "Mortis|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Mortis|Damage")
	FGameplayTag HitEventTag = MortisGameplayTags::Event_Combat_AttackHit;

	UPROPERTY(EditDefaultsOnly, Category = "Mortis|Combat")
	FGameplayTag ComboTransitionEventTag = MortisGameplayTags::Event_Combat_Combo_Next;

	UPROPERTY(EditDefaultsOnly, Category = "Mortis|Damage")
	FGameplayTag DamageTag = MortisGameplayTags::Data_Enemy_Stat_WeaponDamage;
	
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

	UFUNCTION()
	void OnHitEventReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboTransitionReceived(FGameplayEventData Payload);
	
private:
	UPROPERTY(VisibleAnywhere)
	int32 CurrentStepIndex = 0;

	const FMortisAttackPattern* AttackPattern;
	
	TWeakObjectPtr<UMotionWarpingComponent> CachedMotionWarpingComp;
	TWeakObjectPtr<AActor> CachedTargetActor;
	TWeakObjectPtr<UAbilityTask_PlayMontageAndWait> CachedMontageTask;
	TWeakObjectPtr<UMortisAT_UpdateWarpTarget> CachedUpdateWarpTargetTask;
	TWeakObjectPtr<UAbilityTask_WaitGameplayEvent> CachedWaitHitTask; 
	TWeakObjectPtr<UAbilityTask_WaitGameplayEvent> CachedComboTransitionTask; 

	FName LastWarpTargetName = NAME_None;

	void ResetCachedTasks();
};
