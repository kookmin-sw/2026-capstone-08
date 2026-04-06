// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "MortisInteractionComponent.generated.h"

class AMortisInteractableActorBase;
class APawn;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisInteractionComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void LockSelectedAsPending();

	UFUNCTION(BlueprintCallable)
	void ClearPendingTarget();

	UFUNCTION(BlueprintCallable)
	void SelectNextTarget();

	UFUNCTION(BlueprintCallable)
	void SelectPreviousTarget();

	UFUNCTION(BlueprintCallable)
	void ClearAllCandidates();

	UFUNCTION(BlueprintCallable)
	void CleanupInvalidCandidates();

	UFUNCTION(BlueprintPure)
	AMortisInteractableActorBase* GetSelectedTarget() const { return SelectedTarget; }

	UFUNCTION(BlueprintPure)
	AMortisInteractableActorBase* GetPendingTarget() const { return PendingTarget; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TArray<TObjectPtr<AMortisInteractableActorBase>> InteractionCandidates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<AMortisInteractableActorBase> SelectedTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<AMortisInteractableActorBase> PendingTarget = nullptr;

protected:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetSelectedTarget(AMortisInteractableActorBase* NewTarget);
	void AddCandidate(AMortisInteractableActorBase* NewTarget);
	void RemoveCandidate(AMortisInteractableActorBase* RemoveTarget);
	void ReselectIfNeeded();

	bool IsTargetStillUsable(AMortisInteractableActorBase* Target) const;
};
