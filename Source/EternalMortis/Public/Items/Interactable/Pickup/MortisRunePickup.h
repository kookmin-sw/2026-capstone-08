// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Interactable/Pickup/MortisPickupBase.h"
#include "Types/MortisRuneDataTypes.h"
#include "MortisRunePickup.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisRunePickup : public AMortisPickupBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetRuneData(const FMortisRuneInstance& InRuneData);

	UFUNCTION(BlueprintPure)
	const FMortisRuneInstance& GetRuneData() const { return RuneData; }

	UFUNCTION(BlueprintCallable)
	void ReactivateAsDroppedRune(const FMortisRuneInstance& InOldRuneData, const FVector& InStartLocation, const FVector& InTargetLocation, float InDuration = -1.f, float InArcHeight = -1.f);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rune")
	FMortisRuneInstance RuneData;
};
