// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Interactable/Pickup/MortisWeaponPickup.h"
#include "Types/MortisStructTypes.h"
#include "Types/MortisEnumTypes.h"
#include "Types/MortisWeaponDataTypes.h"
#include "Interfaces/MortisShopItemInterface.h"
#include "MortisWeaponShopItem.generated.h"

class AMortisShopkeeperCharacter;
class UGameplayAbility;
struct FMortisWeaponRow;

UCLASS()
class ETERNALMORTIS_API AMortisWeaponShopItem : public AMortisWeaponPickup, public IMortisShopItemInterface
{
	GENERATED_BODY()

public:
	AMortisWeaponShopItem();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// AMortisInteractableActorBase
	virtual bool CanInteract(APawn* InteractingPawn) const override;
	virtual bool CanBeInteractionCandidate(APawn* InteractingPawn) const override;
	virtual void OnInteractionReserved(APawn* InteractingPawn) override;
	virtual void OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded) override;

	// IMortisShopItemInterface
	virtual FMortisShopItemState GetShopState_Implementation() const override;
	virtual AMortisShopkeeperCharacter* GetOwningShopkeeper_Implementation() const override;
	virtual bool TryPrepareTransaction_Implementation(APawn* InteractingPawn, EMortisShopTransactionType TransactionType) override;
	virtual void CancelPreparedTransaction_Implementation() override;
	virtual void SetShopPrice_Implementation(int32 NewPrice) override;
	virtual TSubclassOf<UGameplayAbility> GetStealInteractionAbility_Implementation() const override;

	UFUNCTION(BlueprintCallable)
	void InitializeWeaponShopItem(const FMortisWeaponRow& InWeaponData, int32 InPrice, AMortisShopkeeperCharacter* InShopkeeper);

	UFUNCTION(BlueprintCallable)
	void SetOwningShopkeeper(AMortisShopkeeperCharacter* NewShopkeeper);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Shop|Weapon")
	void BP_ApplyShopWeaponData();
	virtual void BP_ApplyShopWeaponData_Implementation();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	FMortisShopItemState ShopState;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Shop")
	TObjectPtr<AMortisShopkeeperCharacter> OwningShopkeeper = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Ability")
	TSubclassOf<UGameplayAbility> StealAbilityClass;
};