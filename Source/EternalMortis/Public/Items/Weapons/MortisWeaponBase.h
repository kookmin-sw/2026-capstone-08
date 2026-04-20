// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "MortisWeaponBase.generated.h"

DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate, AActor*)

UCLASS()
class ETERNALMORTIS_API AMortisWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMortisWeaponBase();

	FOnTargetInteractedDelegate OnWeaponHitTarget;
	FOnTargetInteractedDelegate OnWeaponPulledFromTarget;
	
	UFUNCTION(BlueprintCallable)
	virtual UMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
	const TArray<TObjectPtr<UShapeComponent>>* GetCollisionComponentsByTag(FGameplayTag TagToToggle);
	
	void ToggleCollision(bool bEnable, const FGameplayTag& TagToToggle);
	
protected:
	virtual void BeginPlay() override;
	
	void InitializeCollisions();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Weapon")
	TObjectPtr<USceneComponent> WeaponRoot;
	
	UPROPERTY()
	TObjectPtr<UMeshComponent> WeaponMesh;
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|Weapon")
	TMap<FGameplayTag, TArray<TObjectPtr<UShapeComponent>>> CollisionComponentMap;
	
	UFUNCTION()
	virtual void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void InitializeWeaponMesh();
};
