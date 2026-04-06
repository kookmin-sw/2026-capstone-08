// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Interactable/PointInteractable/MortisPointInteractableActorBase.h"
#include "MortisChest.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisChest : public AMortisPointInteractableActorBase
{
	GENERATED_BODY()
	
public:
	AMortisChest();

	virtual bool CanInteract(APawn* InteractingPawn) const override;
	virtual void OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chest")
	bool bAlreadyOpened = false;
};
