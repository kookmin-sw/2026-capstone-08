// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Player/Interact/MortisGA_PlayerBuyItem.h"

#include "Character/Player/MortisPlayerCharacter.h"
#include "Components/Collisions/MortisInteractionComponent.h"
#include "Interfaces/MortisShopItemInterface.h"
#include "Items/Interactable/MortisInteractableActorBase.h"
#include "Types/MortisEnumTypes.h"

UMortisGA_PlayerBuyItem::UMortisGA_PlayerBuyItem()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UMortisGA_PlayerBuyItem::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	ClearPreparedShopPurchase();

	if (!TryPrepareShopPurchase())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMortisGA_PlayerBuyItem::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (bHasPreparedShopPurchase && !bPreparedShopPurchaseFinished)
	{
		FinishPreparedShopPurchase(false);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMortisGA_PlayerBuyItem::HandlePreparedShopTargetDestroyed(AActor* DestroyedActor)
{
	FinishPreparedShopPurchase(true, DestroyedActor);
}

bool UMortisGA_PlayerBuyItem::TryPrepareShopPurchase()
{
	AMortisPlayerCharacter* PlayerCharacter = GetMortisPlayerCharacterFromActorInfo();
	UMortisInteractionComponent* InteractionComponent = PlayerCharacter
		? PlayerCharacter->FindComponentByClass<UMortisInteractionComponent>()
		: nullptr;

	AMortisInteractableActorBase* Target = InteractionComponent ? InteractionComponent->GetPendingTarget() : nullptr;
	if (!Target && InteractionComponent)
	{
		Target = InteractionComponent->GetSelectedTarget();
	}

	if (!PlayerCharacter || !Target || !Target->GetClass()->ImplementsInterface(UMortisShopItemInterface::StaticClass()))
	{
		return false;
	}

	if (!IMortisShopItemInterface::Execute_TryPrepareTransaction(Target, PlayerCharacter, EMortisShopTransactionType::Purchase))
	{
		return false;
	}

	PreparedShopTarget = Target;
	bHasPreparedShopPurchase = true;
	bPreparedShopPurchaseFinished = false;
	Target->OnDestroyed.AddDynamic(this, &ThisClass::HandlePreparedShopTargetDestroyed);

	return true;
}

void UMortisGA_PlayerBuyItem::FinishPreparedShopPurchase(bool bSucceeded, AActor* ExplicitTarget)
{
	if (!bHasPreparedShopPurchase || bPreparedShopPurchaseFinished)
	{
		return;
	}

	bPreparedShopPurchaseFinished = true;

	AMortisInteractableActorBase* Target = Cast<AMortisInteractableActorBase>(
		ExplicitTarget ? ExplicitTarget : PreparedShopTarget.Get());
	AMortisPlayerCharacter* PlayerCharacter = GetMortisPlayerCharacterFromActorInfo();

	if (Target && Target->GetClass()->ImplementsInterface(UMortisShopItemInterface::StaticClass()))
	{
		Target->OnInteractionFinished(PlayerCharacter, bSucceeded);
	}

	ClearPreparedShopPurchase();
}

void UMortisGA_PlayerBuyItem::ClearPreparedShopPurchase()
{
	if (AMortisInteractableActorBase* Target = PreparedShopTarget.Get())
	{
		Target->OnDestroyed.RemoveDynamic(this, &ThisClass::HandlePreparedShopTargetDestroyed);
	}

	PreparedShopTarget.Reset();
	bHasPreparedShopPurchase = false;
	bPreparedShopPurchaseFinished = false;
}
