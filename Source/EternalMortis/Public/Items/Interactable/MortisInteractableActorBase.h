// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MortisInteractableActorBase.generated.h"

class APawn;
class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UGameplayAbility;

UCLASS(Abstract)
class ETERNALMORTIS_API AMortisInteractableActorBase : public AActor
{
	GENERATED_BODY()
	
public:
	AMortisInteractableActorBase();

	UFUNCTION(BlueprintCallable)
	virtual bool CanInteract(APawn* InteractingPawn) const;

	UFUNCTION(BlueprintCallable)
	virtual TSubclassOf<UGameplayAbility> GetInteractionAbility() const;

	UFUNCTION(BlueprintCallable)
	virtual void OnInteractionReserved(APawn* InteractingPawn);

	UFUNCTION(BlueprintCallable)
	virtual void OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded);

	UFUNCTION(BlueprintCallable)
	virtual void EnableInteraction();

	UFUNCTION(BlueprintCallable)
	virtual void DisableInteraction();

	UFUNCTION(BlueprintPure)
	bool IsInteractionEnabled() const;

	UFUNCTION(BlueprintCallable)
	virtual void SetSelectionUIVisible(bool bVisible);

	FORCEINLINE UBoxComponent* GetInteractionCollision() const { return InteractionCollision; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UBoxComponent> InteractionCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bCanInteract = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bDisableOnReserve = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TSubclassOf<UGameplayAbility> InteractionAbilityClass;

};
