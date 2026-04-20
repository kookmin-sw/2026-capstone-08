// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawn/MortisSpawnSubsystem.h"

#include "MortisDebugHelper.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Spawn/MortisRoomEnemySpawner.h"
#include "Spawn/MortisSpawnSettings.h"
#include "System/MortisGameState.h"

void UMortisSpawnSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	// AssignEnemies();
}

void UMortisSpawnSubsystem::InitializeSpawnPoints()
{
	AssignEnemies();
}

void UMortisSpawnSubsystem::AssignEnemies()
{
	AMortisGameState* GS = GetWorld()->GetGameState<AMortisGameState>();;
	if (!GS)
	{
		MORTIS_LOG("GameState is null");
		return;
	}
	int32 CurrentFloor = GS->GetCurrentFloor();
	
	const UMortisSpawnSettings* SpawnSettings = GetDefault<UMortisSpawnSettings>();
	if (!SpawnSettings)
	{
		MORTIS_LOG("Spawn Settings is null");
		return;	
	}
	
	const FDataTableRowHandle& FloorEnemySpawnRowHandle = SpawnSettings->FloorSpawnPools[CurrentFloor];
	FMortisFloorSpawnRow* FloorSpawnRow = FloorEnemySpawnRowHandle.GetRow<FMortisFloorSpawnRow>(TEXT("FloorSpawnRow"));
	if (!FloorSpawnRow)
	{
		MORTIS_LOG("FloorSpawnRow is null");
		return;
	}
	
	TArray<AActor*> FoundRoomEnemySpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMortisRoomEnemySpawner::StaticClass(), FoundRoomEnemySpawners);
	// MORTIS_LOG("Spawner Counts: %d", FoundRoomEnemySpawners.Num());
	
	for (AActor* EnemySpawnerActor : FoundRoomEnemySpawners)
	{
		AMortisRoomEnemySpawner* RoomEnemySpawner = Cast<AMortisRoomEnemySpawner>(EnemySpawnerActor);
		if (!RoomEnemySpawner)
		{
			continue;
		}
		switch (RoomEnemySpawner->GetSpawnMode())
		{
		case EMortisSpawnMode::RandomByFloor:
			AssignEnemiesByFloor(RoomEnemySpawner, FloorSpawnRow, FloorSpawnRow->SpawnBudget);
			break;
		case EMortisSpawnMode::FixedCustom:
			AssignEnemiesFromCustomRow(RoomEnemySpawner, RoomEnemySpawner->GetCustomSpawnRowHandle());
			break;
		default:
			break;
		}
	}
}

void UMortisSpawnSubsystem::AssignEnemiesByFloor(AMortisRoomEnemySpawner* RoomSpawner,
	const FMortisFloorSpawnRow* FloorSpawnRow, float SpawnBudget)
{
	// MORTIS_LOG("Assign Enemy By Floor");
	if (!FloorSpawnRow)
	{
		MORTIS_LOG("Spawn Row is null");
		return;
	}
	TArray<TSubclassOf<AMortisEnemyCharacter>> SelectedEnemies = SelectEnemiesToSpawn(FloorSpawnRow->EnemyRows, SpawnBudget);
	RoomSpawner->SetEnemiesToSpawn(SelectedEnemies);
}

void UMortisSpawnSubsystem::AssignEnemiesFromCustomRow(AMortisRoomEnemySpawner* RoomSpawner,
	const FDataTableRowHandle& CustomSpawnRowHandle)
{
	if (CustomSpawnRowHandle.IsNull())
	{
		return;
	}
	FMortisRoomCustomSpawnRow* CustomSpawnRow = CustomSpawnRowHandle.GetRow<FMortisRoomCustomSpawnRow>(TEXT("SpawnCustomSpawnRow"));
	if (!CustomSpawnRow)
	{
		return;
	}
	
	TArray<FDataTableRowHandle> EnemyRowHandles = CustomSpawnRow->EnemyRows;
	TArray<TSubclassOf<AMortisEnemyCharacter>> EnemiesToSpawn;
	for (const FDataTableRowHandle& EnemyRowHandle : EnemyRowHandles)
	{
		FMortisSpawnEnemyRow* EnemyRow = EnemyRowHandle.GetRow<FMortisSpawnEnemyRow>(TEXT("SpawnEnemyRow"));
		EnemiesToSpawn.Add(EnemyRow->EnemyClass);
	}
	RoomSpawner->SetEnemiesToSpawn(EnemiesToSpawn);
}

TArray<TSubclassOf<AMortisEnemyCharacter>> UMortisSpawnSubsystem::SelectEnemiesToSpawn(
	const TArray<FDataTableRowHandle>& EnemyPool, float Budget)
{
	// MORTIS_LOG("Select Enemies");
	TArray<TSubclassOf<AMortisEnemyCharacter>> SelectedEnemies;
	if (EnemyPool.IsEmpty())
	{
		MORTIS_LOG("Enemy Pool is null");
		return SelectedEnemies;
	}
	float SpawnCostSum = 0.f;
	while (SpawnCostSum < Budget)
	{
		int32 RandomIndex = FMath::RandRange(0, EnemyPool.Num() - 1);
		FMortisSpawnEnemyRow* EnemyRow = EnemyPool[RandomIndex].GetRow<FMortisSpawnEnemyRow>(TEXT("SpawnEnemyRow"));
		if (!EnemyRow)
		{
			continue;
		}
		// MORTIS_LOG("Select Enemy!");
		
		SelectedEnemies.Add(EnemyRow->EnemyClass);
		SpawnCostSum += EnemyRow->SpawnCost;
	}
	return SelectedEnemies;
}
