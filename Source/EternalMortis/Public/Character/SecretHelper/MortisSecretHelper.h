// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MortisSecretHelper.generated.h"

class AMortisPickupBase;
class UBillboardComponent;
class USceneComponent;

UCLASS()
class ETERNALMORTIS_API AMortisSecretHelper : public AActor
{
	GENERATED_BODY()
	
public:
	AMortisSecretHelper();

	UFUNCTION(BlueprintCallable, Category = "Mortis|Secret Helper")
	void RegisterOwnedPickup(AMortisPickupBase* Pickup);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Secret Helper")
	void UnregisterOwnedPickup(AMortisPickupBase* Pickup);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Secret Helper")
	void NotifyOwnedPickupInteracted(AMortisPickupBase* InteractedPickup, APawn* InteractingPawn);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|Secret Helper")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|Secret Helper|Spawn")
	TObjectPtr<USceneComponent> PickupSpawnPoint_0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|Secret Helper|Spawn")
	TObjectPtr<USceneComponent> PickupSpawnPoint_1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|Secret Helper|Spawn")
	TObjectPtr<USceneComponent> PickupSpawnPoint_2;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Mortis|Secret Helper")
	TArray<TObjectPtr<AMortisPickupBase>> OwnedPickups;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Secret Helper")
	bool bDestroySelfAfterSelection = true;

private:
	bool bSelectionResolved = false;

};
