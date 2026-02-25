// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/MortisEnemyGameplayAbility.h"
#include "Types/MortisStructTypes.h"
#include "MortisGA_ExecuteAttackPattern.generated.h"

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
	//~ Begin GameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//~ End GameplayAbility Interface

protected:
	UPROPERTY(EditAnywhere)
	FName WarpTargetName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FMortisAttackPattern AttackPattern;
	
	UFUNCTION(BlueprintCallable)
	void ExecuteNextStep();

	UFUNCTION()
	void OnStepMontageCompleted();

	UFUNCTION()
	void OnStepMontageInterrupted();
	
	UFUNCTION(BlueprintCallable)
	void OnStepFinished(bool bInterrupted);
	
private:
	UPROPERTY(VisibleAnywhere)
	int32 CurrentStepIndex = 0;
};
