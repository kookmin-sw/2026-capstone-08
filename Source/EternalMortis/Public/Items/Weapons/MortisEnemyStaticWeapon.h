// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/MortisEnemyWeapon.h"
#include "MortisEnemyStaticWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisEnemyStaticWeapon : public AMortisEnemyWeapon
{
	GENERATED_BODY()

public:
	AMortisEnemyStaticWeapon();

	FORCEINLINE virtual UMeshComponent* GetWeaponMesh() const override { return WeaponMesh; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
};
