// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/MortisShopkeeperCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Items/Interactable/Pickup/ShopItem/MortisWeaponShopItem.h"
#include "Items/Interactable/Pickup/ShopItem/MortisRuneShopItem.h"
#include "System/MortisShopPricingSubsystem.h"
#include "System/MortisRunStateSubsystem.h"

#include "MortisDebugHelper.h"

AMortisShopkeeperCharacter::AMortisShopkeeperCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 생성자가 왜 필요한거지 대체
}

void AMortisShopkeeperCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetAggroState(false, nullptr);
	BP_GenerateShopStock();
}

void AMortisShopkeeperCharacter::RegisterWeaponItem(AMortisWeaponShopItem* ShopItem)
{
	if (!IsValid(ShopItem))
		return;

	RegisteredWeaponItems.AddUnique(ShopItem);
}

void AMortisShopkeeperCharacter::UnregisterWeaponItem(AMortisWeaponShopItem* ShopItem)
{
	if (!IsValid(ShopItem))
		return;

	RegisteredWeaponItems.RemoveSingleSwap(ShopItem);
}

void AMortisShopkeeperCharacter::RegisterRuneItem(AMortisRuneShopItem* ShopItem)
{
	if (!IsValid(ShopItem))
		return;

	RegisteredRuneItems.AddUnique(ShopItem);
}

void AMortisShopkeeperCharacter::UnregisterRuneItem(AMortisRuneShopItem* ShopItem)
{
	if (!IsValid(ShopItem))
		return;

	RegisteredRuneItems.RemoveSingleSwap(ShopItem);
}

bool AMortisShopkeeperCharacter::CanPurchase(APawn* InteractingPawn, int32 Price, bool bAlreadySold) const
{
	if (!IsValid(InteractingPawn))
		return false;

	if (bAggroed)
		return false;

	if (bAlreadySold)
		return false;

	UMortisRunStateSubsystem* RunState = GetRunStateSubsystem();
	if (!RunState)
		return false;
	
	return RunState->HasEnoughGold(Price);
}

bool AMortisShopkeeperCharacter::CanSteal(APawn* InteractingPawn, bool bAlreadySold, bool bCanSteal) const
{
	if (!IsValid(InteractingPawn))
		return false;

	if (bAlreadySold)
		return false;

	if (!bCanSteal)
		return false;

	return true;
}

bool AMortisShopkeeperCharacter::CommitPurchase(APawn* InteractingPawn, int32 Price)
{
	if (!IsValid(InteractingPawn))
		return false;

	UMortisRunStateSubsystem* RunState = GetRunStateSubsystem();
	if (!RunState)
		return false;

	return RunState->SpendGold(Price);
}

bool AMortisShopkeeperCharacter::CommitSteal(APawn* InteractingPawn)
{
	if (!IsValid(InteractingPawn))
		return false;

	TriggerAggro(InteractingPawn);
	return true;
}

void AMortisShopkeeperCharacter::TriggerAggro(APawn* AggroTarget)
{
	/*SetAggroState(true, AggroTarget);
	BP_OnAggroTriggered(AggroTarget);*/
	MORTIS_LOG("ShopKeeper Angry!");
}

void AMortisShopkeeperCharacter::SetAggroState(bool bNewAggroed, APawn* AggroTarget)
{
	bAggroed = bNewAggroed;

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent())
		{
			Blackboard->SetValueAsBool(BlackboardAggroKey, bAggroed);
			Blackboard->SetValueAsObject(BlackboardTargetActorKey, AggroTarget);
		}
	}
}

UMortisRunStateSubsystem* AMortisShopkeeperCharacter::GetRunStateSubsystem() const
{
	if (!GetGameInstance())
		return nullptr;

	return GetGameInstance()->GetSubsystem<UMortisRunStateSubsystem>();
}

UMortisShopPricingSubsystem* AMortisShopkeeperCharacter::GetShopPricingSubsystem() const
{
	if (!GetGameInstance())
		return nullptr;

	return GetGameInstance()->GetSubsystem<UMortisShopPricingSubsystem>();
}

void AMortisShopkeeperCharacter::GenerateShopStock()
{
	BP_GenerateShopStock();
}

bool AMortisShopkeeperCharacter::GenerateWeaponShopItemPrice(const FMortisWeaponRow& WeaponData, int32& OutPrice) const
{
	OutPrice = 0;

	UMortisShopPricingSubsystem* PricingSubsystem = GetShopPricingSubsystem();
	if (!PricingSubsystem)
	{
		return false;
	}

	return PricingSubsystem->GenerateWeaponPrice(WeaponData, OutPrice);
}

bool AMortisShopkeeperCharacter::GenerateRuneShopItemPrice(const FMortisRuneInstance& RuneInstance, int32& OutPrice) const
{
	OutPrice = 0;

	UMortisShopPricingSubsystem* PricingSubsystem = GetShopPricingSubsystem();
	if (!PricingSubsystem)
	{
		return false;
	}

	return PricingSubsystem->GenerateRunePrice(RuneInstance, OutPrice);
}

bool AMortisShopkeeperCharacter::InitializeWeaponShopItemWithGeneratedPrice(AMortisWeaponShopItem* ShopItem, const FMortisWeaponRow& WeaponData)
{
	if (!IsValid(ShopItem))
	{
		return false;
	}

	int32 GeneratedPrice = 0;
	if (!GenerateWeaponShopItemPrice(WeaponData, GeneratedPrice))
	{
		return false;
	}

	ShopItem->InitializeWeaponShopItem(WeaponData, GeneratedPrice, this);
	return true;
}

bool AMortisShopkeeperCharacter::InitializeRuneShopItemWithGeneratedPrice(AMortisRuneShopItem* ShopItem, const FMortisRuneInstance& RuneInstance)
{
	if (!IsValid(ShopItem))
	{
		return false;
	}

	int32 GeneratedPrice = 0;
	if (!GenerateRuneShopItemPrice(RuneInstance, GeneratedPrice))
	{
		return false;
	}

	ShopItem->InitializeRuneShopItem(RuneInstance, GeneratedPrice, this);
	return true;
}

void AMortisShopkeeperCharacter::BP_GenerateShopStock_Implementation()
{
	// C++에서 DB 연동 전엔 BP에서 처리하는 게 제일 편함.
	// 예:
	// InitializeWeaponShopItemWithGeneratedPrice(WeaponSlots[0], RandomWeaponA);
	// InitializeWeaponShopItemWithGeneratedPrice(WeaponSlots[1], RandomWeaponB);
	// InitializeRuneShopItemWithGeneratedPrice(RuneSlots[0], RandomRuneA);
}

void AMortisShopkeeperCharacter::BP_OnAggroTriggered_Implementation(APawn* AggroTarget)
{
	// BP에서 대사, 이펙트, 전투 시작 연출
}
