// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Interactable/Pickup/MortisWeaponPickup.h"
#include "MortisSecretHelperWeaponPickup.generated.h"

class AMortisSecretHelper;

UCLASS()
class ETERNALMORTIS_API AMortisSecretHelperWeaponPickup : public AMortisWeaponPickup
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Mortis|Secret Helper")
	void SetOwningSecretHelper(AMortisSecretHelper* InHelper);

protected:
	virtual void OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded) override;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Mortis|Secret Helper")
	TObjectPtr<AMortisSecretHelper> OwningSecretHelper;
};
