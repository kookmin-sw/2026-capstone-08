// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Pickup/ShopItem/MortisRuneShopItem.h"
#include "Character/Enemy/MortisShopkeeperCharacter.h"
#include "Abilities/GameplayAbility.h"
#include "Components/WidgetComponent.h"
#include "UI/MortisPickupPreviewWidget.h"

namespace
{
	FText BuildRuneShopPickupTitleText(const FText& ItemName, int32 Price)
	{
		return FText::Format(
			NSLOCTEXT("MortisShopItem", "PickupTitleWithGoldPrice", "{0}\n{1} Gold"),
			ItemName,
			FText::AsNumber(FMath::Max(0, Price)));
	}
}

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
	DisableInteraction();
	return true;
}

void AMortisRuneShopItem::CancelPreparedTransaction_Implementation()
{
	ShopState.PendingTransaction = EMortisShopTransactionType::None;
	EnableInteraction();
}

void AMortisRuneShopItem::SetShopPrice_Implementation(int32 NewPrice)
{
	ShopState.Price = FMath::Max(0, NewPrice);
	RefreshShopPreviewIfVisible();
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

bool AMortisRuneShopItem::BuildPickupPreviewData(FMortisPickupPreviewData& OutPreviewData) const
{
	if (!Super::BuildPickupPreviewData(OutPreviewData))
	{
		return false;
	}

	OutPreviewData.TitleText = BuildRuneShopPickupTitleText(OutPreviewData.TitleText, ShopState.Price);
	return true;
}

void AMortisRuneShopItem::InitializeRuneShopItem(const FMortisRuneInstance& InRuneInstance, int32 InPrice, AMortisShopkeeperCharacter* InShopkeeper)
{
	SetRuneData(InRuneInstance);
	ShopState.Price = FMath::Max(0, InPrice);
	ShopState.ResetRuntimeState();

	if (OwningShopkeeper != InShopkeeper)
	{
		if (HasActorBegunPlay() && OwningShopkeeper)
			OwningShopkeeper->UnregisterRuneItem(this);

		OwningShopkeeper = InShopkeeper;

		if (HasActorBegunPlay() && OwningShopkeeper)
			OwningShopkeeper->RegisterRuneItem(this);
	}

	ShopState.PendingTransaction = EMortisShopTransactionType::None;

	BP_ApplyShopRuneData();
	RefreshShopPreviewIfVisible();
}

void AMortisRuneShopItem::SetOwningShopkeeper(AMortisShopkeeperCharacter* NewShopkeeper)
{
	if (OwningShopkeeper == NewShopkeeper)
		return;

	if (HasActorBegunPlay() && OwningShopkeeper)
		OwningShopkeeper->UnregisterRuneItem(this);

	OwningShopkeeper = NewShopkeeper;

	if (HasActorBegunPlay() && OwningShopkeeper)
		OwningShopkeeper->RegisterRuneItem(this);
}

void AMortisRuneShopItem::BP_ApplyShopRuneData_Implementation()
{
	// RuneData 기준 비주얼 갱신
}

void AMortisRuneShopItem::RefreshShopPreviewIfVisible()
{
	if (SelectionWidget && SelectionWidget->IsVisible())
	{
		RefreshPickupPreviewWidget();
	}
}
