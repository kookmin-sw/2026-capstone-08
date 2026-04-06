// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Interactable/MortisInteractableActorBase.h"
#include "MortisPointInteractableActorBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ETERNALMORTIS_API AMortisPointInteractableActorBase : public AMortisInteractableActorBase
{
	GENERATED_BODY()
	
public:
	AMortisPointInteractableActorBase();

	UFUNCTION(BlueprintPure)
	USceneComponent* GetInteractionPoint() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<USceneComponent> InteractionPoint;
};
