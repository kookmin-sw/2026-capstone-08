// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Interactable/MortisInteractableActorBase.h"
#include "MortisGateInteractableBase.generated.h"

class UBoxComponent;
class USceneComponent;
class APawn;

UCLASS(Abstract)
class ETERNALMORTIS_API AMortisGateInteractableBase : public AMortisInteractableActorBase
{
	GENERATED_BODY()

public:
	AMortisGateInteractableBase();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual bool CanInteract(APawn* InteractingPawn) const override;
	virtual bool CanBeInteractionCandidate(APawn* InteractingPawn) const override;

	UFUNCTION(BlueprintPure)
	USceneComponent* GetPassEndPoint() const { return PassEndPoint; }

	UFUNCTION(BlueprintPure)
	UBoxComponent* GetGateBlockCollision() const { return GateBlockCollision; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gate")
	TObjectPtr<UBoxComponent> GateBlockCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gate")
	TObjectPtr<USceneComponent> PassEndPoint;

	// 게이트의 Forward 방향 기준, Pawn이 앞쪽에 있어야 하는 최소 dot
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gate", meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float RequiredFrontDot = 0.2f;

	// 상호작용 콜리전을 게이트 앞쪽으로 얼마나 띄울지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gate")
	FVector InteractionOffset = FVector(120.f, 0.f, 0.f);

	// 상호작용 콜리전 크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gate")
	FVector InteractionExtent = FVector(120.f, 150.f, 100.f);

protected:
	UFUNCTION()
	void HandleGateZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void HandleGateZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool IsPawnInsideGateInteractionZone(APawn* InteractingPawn) const;
};