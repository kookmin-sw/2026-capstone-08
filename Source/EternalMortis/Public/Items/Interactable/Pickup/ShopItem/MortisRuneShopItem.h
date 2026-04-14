// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Interactable/Pickup/MortisRunePickup.h"
#include "Types/MortisStructTypes.h"
#include "Types/MortisEnumTypes.h"
#include "Interfaces/MortisShopItemInterface.h"
#include "MortisRuneShopItem.generated.h"

class AMortisShopkeeperCharacter;
class UGameplayAbility;

UCLASS()
class ETERNALMORTIS_API AMortisRuneShopItem : public AMortisRunePickup, public IMortisShopItemInterface
{
	GENERATED_BODY()

public:
	AMortisRuneShopItem();

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
	void InitializeRuneShopItem(const FMortisRuneInstance& InRuneInstance, int32 InPrice, AMortisShopkeeperCharacter* InShopkeeper);

	UFUNCTION(BlueprintCallable)
	void SetOwningShopkeeper(AMortisShopkeeperCharacter* NewShopkeeper);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Shop|Rune")
	void BP_ApplyShopRuneData();
	virtual void BP_ApplyShopRuneData_Implementation();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	FMortisShopItemState ShopState;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Shop")
	TObjectPtr<AMortisShopkeeperCharacter> OwningShopkeeper = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Ability")
	TSubclassOf<UGameplayAbility> StealAbilityClass;
};