// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Interactable/MortisInteractableActorBase.h"
#include "MortisGateInteractableBase.generated.h"

class UBoxComponent;
class USceneComponent;
class APawn;

/**
 * 
 */
UCLASS(Abstract)
class ETERNALMORTIS_API AMortisGateInteractableBase : public AMortisInteractableActorBase
{
	GENERATED_BODY()
	
public:
	AMortisGateInteractableBase();

	virtual bool CanInteract(APawn* InteractingPawn) const override;

	UFUNCTION(BlueprintPure)
	UBoxComponent* GetFrontInteractZone() const { return FrontInteractZone; }

	UFUNCTION(BlueprintPure)
	USceneComponent* GetPassEndPoint() const { return PassEndPoint; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gate")
	TObjectPtr<UBoxComponent> FrontInteractZone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gate")
	TObjectPtr<UBoxComponent> GateBlockCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gate")
	TObjectPtr<USceneComponent> PassEndPoint;

	// true면 게이트 정면 영역에 들어와 있어야만 상호작용 가능
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gate")
	bool bRequireFrontZone = true;

	// true면 게이트 정면 방향에서만 허용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gate")
	bool bOneWayOnly = true;

	// GateForward와 (Gate - Pawn) 방향 dot
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gate")
	float RequiredFrontDot = 0.2f;

protected:
	bool IsPawnInFrontZone(APawn* InteractingPawn) const;
	bool IsPawnOnAllowedSide(APawn* InteractingPawn) const;
};
