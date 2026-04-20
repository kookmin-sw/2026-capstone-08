// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MortisSpawnTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "MortisSpawnSubsystem.generated.h"

class AMortisEnemyCharacter;
class AMortisRoomEnemySpawner;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisSpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	//~ Begin UWorldSubsystem Interfaces
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	//~ End UWorldSubsystem Interfaces
	
	void AssignEnemies();
	void AssignEnemiesByFloor(AMortisRoomEnemySpawner* RoomSpawner, const FMortisFloorSpawnRow* FloorSpawnRow, float SpawnBudget);
	void AssignEnemiesFromCustomRow(AMortisRoomEnemySpawner* RoomSpawner, const FDataTableRowHandle& CustomSpawnRowHandle);
	
private:
	TArray<TSubclassOf<AMortisEnemyCharacter>> SelectEnemiesToSpawn(const TArray<FDataTableRowHandle>& EnemyPool, float Budget);
};
