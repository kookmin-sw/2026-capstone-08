// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/MortisWeaponBase.h"
#include "Types/MortisStructTypes.h"
#include "MortisEnemyWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisEnemyWeapon : public AMortisWeaponBase
{
	GENERATED_BODY()

public:
	FORCEINLINE FMortisEnemyWeaponData GetEnemyWeaponData() const { return EnemyWeaponData; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|WeaponData")
	FMortisEnemyWeaponData EnemyWeaponData;
	
	//~ Begin AMortisWeaponBase Interface
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	//~ End AMortisWeaponBase Interface
};
