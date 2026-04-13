// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Interactable/MortisInteractableActorBase.h"
#include "MortisPickupBase.generated.h"

class UStaticMeshComponent;
struct FMortisPickupPreviewData;
class UWidgetComponent;
class UMortisPickupPreviewWidget;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisPickupBase : public AMortisInteractableActorBase
{
	GENERATED_BODY()
	
public:
	AMortisPickupBase();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	virtual void ReactivateAsDroppedItem();

	UFUNCTION(BlueprintCallable)
	void StartArcMove(const FVector& InStartLocation, const FVector& InTargetLocation, float InDuration = 0.35f, float InArcHeight = 80.f);

	UFUNCTION(BlueprintPure)
	bool IsArcMoving() const { return bArcMoving; }

	// AMortisInteractableActorBase Override
	virtual void SetSelectionIndicatorVisible(bool bVisible) override;

protected:
	virtual bool BuildPickupPreviewData(FMortisPickupPreviewData& OutPreviewData) const;
	void RefreshPickupPreviewWidget();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Arc")
	float DefaultArcDuration = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Arc")
	float DefaultArcHeight = 80.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup|Indicator")
	TObjectPtr<UStaticMeshComponent> SelectionIndicatorMesh;
	
	virtual void OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded) override;

	UMortisPickupPreviewWidget* GetPickupPreviewWidget() const;
	void FinishArcMove();

protected:
	bool bArcMoving = false;
	float ArcElapsed = 0.f;
	float ArcDuration = 0.35f;
	float ArcHeight = 80.f;

	FVector ArcStart = FVector::ZeroVector;
	FVector ArcTarget = FVector::ZeroVector;
};
