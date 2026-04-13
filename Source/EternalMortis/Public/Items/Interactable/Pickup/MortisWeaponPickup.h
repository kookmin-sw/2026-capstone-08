// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Interactable/Pickup/MortisPickupBase.h"
#include "Types/MortisWeaponDataTypes.h"
#include "MortisWeaponPickup.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisWeaponPickup : public AMortisPickupBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetWeaponData(const FMortisWeaponRow& InWeaponData);

	UFUNCTION(BlueprintPure)
	const FMortisWeaponRow& GetWeaponData() const { return WeaponData; }

	UFUNCTION(BlueprintCallable)
	void ReactivateAsDroppedWeapon(const FMortisWeaponRow& InOldWeaponData, const FVector& InStartLocation, const FVector& InTargetLocation, float InDuration = -1.f, float InArcHeight = -1.f);

protected:
	virtual bool BuildPickupPreviewData(FMortisPickupPreviewData& OutPreviewData) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FMortisWeaponRow WeaponData;
};
