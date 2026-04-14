// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "MortisCombatItemBase.generated.h"
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisCombatItemBase : public AActor
{
	GENERATED_BODY()
	
public:
// 	AMortisCombatItemBase();
//
// 	FORCEINLINE virtual UMeshComponent* GetItemMesh() const { return ItemMesh; }
// 	
// 	void ClearOverlappedActors();
// 	const TArray<TObjectPtr<UShapeComponent>>* GetCollisionComponentsByTag(FGameplayTag TagToToggle);
// 	
// 	bool AddUniqueOverlappedActor(AActor* NewActor);
// protected:
// 	//~ Begin Actor Interfaces
// 	virtual void BeginPlay() override;
// 	//~ End Actor Interfaces
// 	
// 	void InitializeCollisions();
// 	
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Weapon")
// 	TObjectPtr<USceneComponent> ItemRoot;
// 	
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|Weapon")
// 	TObjectPtr<UMeshComponent> ItemMesh;
// 	
// 	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|Weapon")
// 	TMap<FGameplayTag, TArray<TObjectPtr<UShapeComponent>>> CollisionComponentMap;
// 	
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis")
// 	TArray<TObjectPtr<AActor>> OverlappedActors;
// 	
// 	UFUNCTION()
// 	virtual void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
// 	
// 	UFUNCTION()
// 	virtual void OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
// 	
// private:
// 	void InitializeItemMesh();
};
