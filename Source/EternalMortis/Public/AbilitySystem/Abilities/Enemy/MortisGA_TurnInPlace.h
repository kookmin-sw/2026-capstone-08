// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/MortisEnemyGameplayAbility.h"
#include "MortisGA_TurnInPlace.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisGA_TurnInPlace : public UMortisEnemyGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> MontageTurnRight90;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> MontageTurnRight180;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> MontageTurnLeft90;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> MontageTurnLeft180;
	
	UPROPERTY(EditAnywhere)
	FName WarpTargetName;

	UPROPERTY(EditAnywhere)
	float Turn90Threshold = 60.f;

	UPROPERTY(EditAnywhere)
	float Turn180Threshold = 120;
	
private:
	UAnimMontage* SelectBestMontage(float AngleToTarget);
	
	UFUNCTION()
	void OnMontageCompleted();
};
