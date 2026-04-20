// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MortisSpawnTypes.generated.h"

class AMortisEnemyCharacter;
/**
 * 
 */
UENUM()
enum class EMortisSpawnPattern : uint8
{
	Point,
	Circle,
	Grid,
	Custom
};

UENUM()
enum class EMortisSpawnMode : uint8
{
	RandomByFloor,
	FixedCustom
};
USTRUCT(BlueprintType)
struct ETERNALMORTIS_API FMortisSpawnEnemyRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<AMortisEnemyCharacter> EnemyClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	float SpawnCost;
};

USTRUCT(BlueprintType)
struct ETERNALMORTIS_API FMortisFloorSpawnRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	int32 Floor = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	float SpawnBudget = 5.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TArray<FDataTableRowHandle> EnemyRows;
};

USTRUCT(BlueprintType)
struct ETERNALMORTIS_API FMortisRoomCustomSpawnRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TArray<FDataTableRowHandle> EnemyRows;
};
