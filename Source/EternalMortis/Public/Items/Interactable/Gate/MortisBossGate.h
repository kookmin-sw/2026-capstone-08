// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Interactable/Gate/MortisGateInteractableBase.h"
#include "MortisBossGate.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisBossGate : public AMortisGateInteractableBase
{
	GENERATED_BODY()
	
public:
	AMortisBossGate();

	virtual bool CanInteract(APawn* InteractingPawn) const override;
	virtual void OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded) override;

	UFUNCTION(BlueprintPure)
	bool HasAlreadyPassed() const { return bAlreadyPassed; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gate")
	bool bAlreadyPassed = false;
};
