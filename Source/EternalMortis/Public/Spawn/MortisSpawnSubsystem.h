// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MortisSpawnTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "MortisSpawnSubsystem.generated.h"

class UMortisEnemySpawnerComponent;
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
	
	UFUNCTION(BlueprintCallable)
	void InitializeSpawnPoints();
	
	void AssignEnemies();
	void AssignEnemiesByFloor(UMortisEnemySpawnerComponent* RoomSpawner, const FMortisFloorSpawnRow* FloorSpawnRow, float SpawnBudget);
	void AssignEnemiesFromCustomRow(UMortisEnemySpawnerComponent* RoomSpawner, const FDataTableRowHandle& CustomSpawnRowHandle);

	UFUNCTION(BlueprintCallable)
	void RegisterSpawner(UMortisEnemySpawnerComponent* NewSpawner);
	
	UFUNCTION(BlueprintCallable)
	void UnregisterSpawner(UMortisEnemySpawnerComponent* Spawner);
private:
	TArray<TWeakObjectPtr<UMortisEnemySpawnerComponent>> SpawnerComponents;
	TArray<TSubclassOf<AMortisEnemyCharacter>> SelectEnemiesToSpawn(const TArray<FDataTableRowHandle>& EnemyPool, float Budget);
};
