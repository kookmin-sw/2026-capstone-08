// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Types/MortisStructTypes.h"
#include "Types/MortisEnumTypes.h"
#include "MortisShopItemInterface.generated.h"

class AMortisShopkeeperCharacter;
class UGameplayAbility;

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UMortisShopItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ETERNALMORTIS_API IMortisShopItemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Mortis|Shop")
	FMortisShopItemState GetShopState() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Mortis|Shop")
	AMortisShopkeeperCharacter* GetOwningShopkeeper() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Mortis|Shop")
	bool TryPrepareTransaction(APawn* InteractingPawn, EMortisShopTransactionType TransactionType);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Mortis|Shop")
	void CancelPreparedTransaction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Mortis|Shop")
	void SetShopPrice(int32 NewPrice);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Mortis|Shop")
	TSubclassOf<UGameplayAbility> GetStealInteractionAbility() const;

};
