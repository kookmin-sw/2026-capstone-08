// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/MortisPlayerGameplayAbility.h"
#include "MortisGA_PlayerBuyItem.generated.h"

class AMortisInteractableActorBase;

/**
 * Wraps the existing Blueprint buy item flow with shop transaction lifetime.
 *
 * The Blueprint still grants the weapon/rune and destroys the shop item on
 * success. This ability prepares the purchase first, commits it when that
 * prepared target is destroyed, and cancels it if the Blueprint exits without
 * consuming the item.
 */
UCLASS()
class ETERNALMORTIS_API UMortisGA_PlayerBuyItem : public UMortisPlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UMortisGA_PlayerBuyItem();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

protected:
	UFUNCTION()
	void HandlePreparedShopTargetDestroyed(AActor* DestroyedActor);

private:
	bool TryPrepareShopPurchase();
	void FinishPreparedShopPurchase(bool bSucceeded, AActor* ExplicitTarget = nullptr);
	void ClearPreparedShopPurchase();

	TWeakObjectPtr<AMortisInteractableActorBase> PreparedShopTarget;
	bool bHasPreparedShopPurchase = false;
	bool bPreparedShopPurchaseFinished = false;
};
