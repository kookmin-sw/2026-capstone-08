// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Pickup/ShopItem/MortisWeaponShopItem.h"
#include "Character/Enemy/MortisShopkeeperCharacter.h"
#include "Abilities/GameplayAbility.h"

AMortisWeaponShopItem::AMortisWeaponShopItem()
{
	bDisableOnReserve = false;
}

void AMortisWeaponShopItem::BeginPlay()
{
	Super::BeginPlay();

	if (OwningShopkeeper)
		OwningShopkeeper->RegisterWeaponItem(this);
}

void AMortisWeaponShopItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OwningShopkeeper)
		OwningShopkeeper->UnregisterWeaponItem(this);

	Super::EndPlay(EndPlayReason);
}

bool AMortisWeaponShopItem::CanInteract(APawn* InteractingPawn) const
{
	if (!Super::CanInteract(InteractingPawn))
		return false;

	if (ShopState.bSold)
		return false;

	if (ShopState.PendingTransaction != EMortisShopTransactionType::None)
		return false;

	return GetInteractionAbility() != nullptr;
}

bool AMortisWeaponShopItem::CanBeInteractionCandidate(APawn* InteractingPawn) const
{
	return CanInteract(InteractingPawn);
}

void AMortisWeaponShopItem::OnInteractionReserved(APawn* InteractingPawn)
{
	// 거래는 TryPrepareTransaction에서 제어
}

FMortisShopItemState AMortisWeaponShopItem::GetShopState_Implementation() const
{
	return ShopState;
}

AMortisShopkeeperCharacter* AMortisWeaponShopItem::GetOwningShopkeeper_Implementation() const
{
	return OwningShopkeeper;
}

bool AMortisWeaponShopItem::TryPrepareTransaction_Implementation(APawn* InteractingPawn, EMortisShopTransactionType TransactionType)
{
	if (!OwningShopkeeper || !CanInteract(InteractingPawn))
		return false;

	bool bAllowed = false;

	switch (TransactionType)
	{
	case EMortisShopTransactionType::Purchase:
		bAllowed = GetInteractionAbility() != nullptr &&
			OwningShopkeeper->CanPurchase(InteractingPawn, ShopState.Price, ShopState.bSold);
		break;

	case EMortisShopTransactionType::Steal:
		bAllowed = StealAbilityClass != nullptr &&
			OwningShopkeeper->CanSteal(InteractingPawn, ShopState.bSold, ShopState.bCanSteal);
		break;

	default:
		break;
	}

	if (!bAllowed)
	{
		return false;
	}

	ShopState.PendingTransaction = TransactionType;
	DisableInteraction();
	return true;
}

void AMortisWeaponShopItem::CancelPreparedTransaction_Implementation()
{
	ShopState.PendingTransaction = EMortisShopTransactionType::None;

}

void AMortisWeaponShopItem::SetShopPrice_Implementation(int32 NewPrice)
{
	ShopState.Price = FMath::Max(0, NewPrice);
}

TSubclassOf<UGameplayAbility> AMortisWeaponShopItem::GetStealInteractionAbility_Implementation() const
{
	return StealAbilityClass;
}

void AMortisWeaponShopItem::OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded)
{
	if (ShopState.PendingTransaction == EMortisShopTransactionType::None)
		return;

	if (!bSucceeded)
	{
		CancelPreparedTransaction_Implementation();
		return;
	}

	if (!OwningShopkeeper)
	{
		CancelPreparedTransaction_Implementation();
		return;
	}

	const EMortisShopTransactionType FinishedTransaction = ShopState.PendingTransaction;
	ShopState.PendingTransaction = EMortisShopTransactionType::None;

	bool bCommitted = false;

	switch (FinishedTransaction)
	{
	case EMortisShopTransactionType::Purchase:
		bCommitted = OwningShopkeeper->CommitPurchase(InteractingPawn, ShopState.Price);
		break;

	case EMortisShopTransactionType::Steal:
		bCommitted = OwningShopkeeper->CommitSteal(InteractingPawn);
		break;

	default:
		break;
	}

	if (!bCommitted)
	{
		return;
	}

	ShopState.bSold = true;
	ShopState.bStolen = (FinishedTransaction == EMortisShopTransactionType::Steal);
}

void AMortisWeaponShopItem::InitializeWeaponShopItem(const FMortisWeaponRow& InWeaponData, int32 InPrice, AMortisShopkeeperCharacter* InShopkeeper)
{
	SetWeaponData(InWeaponData);

	ShopState.ResetRuntimeState();
	ShopState.Price = InPrice;

	if (HasActorBegunPlay() && OwningShopkeeper)
		OwningShopkeeper = InShopkeeper;

	ShopState.PendingTransaction = EMortisShopTransactionType::None;

	BP_ApplyShopWeaponData();
}

void AMortisWeaponShopItem::SetOwningShopkeeper(AMortisShopkeeperCharacter* NewShopkeeper)
{
	OwningShopkeeper = NewShopkeeper;
}

void AMortisWeaponShopItem::BP_ApplyShopWeaponData_Implementation()
{
	// 기존 WeaponData 기준 비주얼 갱신
}