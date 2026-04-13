// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Pickup/ShopItem/MortisRuneShopItem.h"
#include "Character/Enemy/MortisShopkeeperCharacter.h"
#include "Abilities/GameplayAbility.h"

AMortisRuneShopItem::AMortisRuneShopItem()
{
	bDisableOnReserve = false;
}

void AMortisRuneShopItem::BeginPlay()
{
	Super::BeginPlay();

	if (OwningShopkeeper)
		OwningShopkeeper->RegisterRuneItem(this);
}

void AMortisRuneShopItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OwningShopkeeper)
		OwningShopkeeper->UnregisterRuneItem(this);

	Super::EndPlay(EndPlayReason);
}

bool AMortisRuneShopItem::CanInteract(APawn* InteractingPawn) const
{
	if (!Super::CanInteract(InteractingPawn))
		return false;

	if (ShopState.bSold)
		return false;

	if (ShopState.PendingTransaction != EMortisShopTransactionType::None)
		return false;

	return GetInteractionAbility() != nullptr;
}

bool AMortisRuneShopItem::CanBeInteractionCandidate(APawn* InteractingPawn) const
{
	return CanInteract(InteractingPawn);
}

void AMortisRuneShopItem::OnInteractionReserved(APawn* InteractingPawn)
{
	// 상점 거래는 TryPrepareTransaction에서 직접 제어
}

FMortisShopItemState AMortisRuneShopItem::GetShopState_Implementation() const
{
	return ShopState;
}

AMortisShopkeeperCharacter* AMortisRuneShopItem::GetOwningShopkeeper_Implementation() const
{
	return OwningShopkeeper;
}

bool AMortisRuneShopItem::TryPrepareTransaction_Implementation(APawn* InteractingPawn, EMortisShopTransactionType TransactionType)
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
		return false;

	ShopState.PendingTransaction = TransactionType;
	return true;
}

void AMortisRuneShopItem::CancelPreparedTransaction_Implementation()
{
	ShopState.PendingTransaction = EMortisShopTransactionType::None;
}

void AMortisRuneShopItem::SetShopPrice_Implementation(int32 NewPrice)
{
	ShopState.Price = FMath::Max(0, NewPrice);
}

TSubclassOf<UGameplayAbility> AMortisRuneShopItem::GetStealInteractionAbility_Implementation() const
{
	return StealAbilityClass;
}

void AMortisRuneShopItem::OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded)
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

void AMortisRuneShopItem::InitializeRuneShopItem(const FMortisRuneInstance& InRuneInstance, int32 InPrice, AMortisShopkeeperCharacter* InShopkeeper)
{
	RuneData = InRuneInstance;
	ShopState.Price = InPrice;
	ShopState.ResetRuntimeState();
	if (HasActorBegunPlay() && OwningShopkeeper)
		OwningShopkeeper = InShopkeeper;

	BP_ApplyShopRuneData();
}

void AMortisRuneShopItem::SetOwningShopkeeper(AMortisShopkeeperCharacter* NewShopkeeper)
{
	OwningShopkeeper = NewShopkeeper;
}

void AMortisRuneShopItem::BP_ApplyShopRuneData_Implementation()
{
	// RuneData 기준 비주얼 갱신
}