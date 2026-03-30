// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/MortisEnemyWeapon.h"
#include "MortisEnemySkeletalWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisEnemySkeletalWeapon : public AMortisEnemyWeapon
{
	GENERATED_BODY()

public:
	AMortisEnemySkeletalWeapon();

	FORCEINLINE virtual UMeshComponent* GetWeaponMesh() const override { return WeaponMesh; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;
};
