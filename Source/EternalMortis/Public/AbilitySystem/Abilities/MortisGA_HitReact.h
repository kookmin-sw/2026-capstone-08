// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MortisGameplayAbility.h"
#include "MortisGA_HitReact.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisGA_HitReact : public UMortisGameplayAbility
{
	GENERATED_BODY()

protected:
	//~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//~ End UGameplayAbility Interface

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HitReact")
	bool bUseDirectionalReaction = false;

	UPROPERTY(EditDefaultsOnly, Category = "HitReact", meta = (EditCondition = "!bUseDirectionalReaction"))
	bool bFaceAttacker = false;

	UPROPERTY(EditDefaultsOnly, Category = "HitReact|Montage")
	TArray<TObjectPtr<UAnimMontage>> ForwardHitMontages;

	UPROPERTY(EditDefaultsOnly, Category = "HitReact|Montage", meta = (EditCondition = "bUseDirectionalReaction"))
	TArray<TObjectPtr<UAnimMontage>> RightHitMontages;

	UPROPERTY(EditDefaultsOnly, Category = "HitReact|Montage", meta = (EditCondition = "bUseDirectionalReaction"))
	TArray<TObjectPtr<UAnimMontage>> LeftHitMontages;

	UPROPERTY(EditDefaultsOnly, Category = "HitReact|Montage", meta = (EditCondition = "bUseDirectionalReaction"))
	TArray<TObjectPtr<UAnimMontage>> BackHitMontages;

private:
	UAnimMontage* SelectHitReactMontage(float AngleToAttacker);
	UAnimMontage* SelectRandomMontage(const TArray<UAnimMontage*>& Montages);

	UFUNCTION()
	void OnMontageCompleted();
};
