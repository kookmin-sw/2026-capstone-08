// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MortisGameplayTags.h"
#include "Types/MortisStructTypes.h"
#include "MortisWeaponBase.generated.h"

class UBoxComponent;
class UMortisBoxComponent;

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

	UShapeComponent* GetWeaponCollisionComponent(FGameplayTag TagToToggle);

	FORCEINLINE virtual UMeshComponent* GetWeaponMesh() const { return nullptr; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Weapon")
	TObjectPtr<USceneComponent> WeaponRoot;
	
	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, TObjectPtr<UShapeComponent>> CollisionComponentMap;

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void InitializeWeaponCollisions();
};
