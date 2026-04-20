// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MortisSpawnTypes.h"
#include "GameFramework/Actor.h"
#include "MortisRoomEnemySpawner.generated.h"

class UBoxComponent;

UCLASS()
class ETERNALMORTIS_API AMortisRoomEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AMortisRoomEnemySpawner();
	
	//~ Begin AActor Interfaces
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void Tick(float DeltaTime) override;
	//~ End AActor Interfaces
	
	FORCEINLINE EMortisSpawnMode GetSpawnMode() const { return SpawnMode; }
	FORCEINLINE FDataTableRowHandle GetCustomSpawnRowHandle() const { return CustomEnemySpawnRowHandle; }
	
	UFUNCTION(CallInEditor, Category = "Mortis")
	void GenerateSpawnPoints();
	
	UFUNCTION(CallInEditor, Category = "Mortis")
	void ClearPoints();
	
	TArray<FTransform> GetWorldSpawnTransforms() const;
	
	void SetEnemiesToSpawn(const TArray<TSubclassOf<AMortisEnemyCharacter>>& NewEnemiesToSpawn);
	void SpawnEnemies() const;
	
protected:
	UPROPERTY()
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(VisibleAnywhere, Category = "Mortis|Spawn")
	TObjectPtr<UBillboardComponent> SpriteComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Mortis|Spawn")
	TObjectPtr<UBoxComponent> SpawnTriggerBox;
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Spawn")
	EMortisSpawnPattern SpawnPattern = EMortisSpawnPattern::Custom;
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Spawn", meta = (EditCondition = "SpawnPattern == EMortisSpawnPattern::Circle"))
	float CircleRadius = 500.f;
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Spawn", meta = (EditCondition = "SpawnPattern == EMortisSpawnPattern::Circle"))
	int32 CircleCount = 4;
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Spawn", meta = (EditCondition = "SpawnPattern == EMortisSpawnPattern::Grid"))
	int32 GridRows = 2;
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Spawn", meta = (EditCondition = "SpawnPattern == EMortisSpawnPattern::Grid"))
	int32 GridCols = 3;
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Spawn", meta = (EditCondition = "SpawnPattern == EMortisSpawnPattern::Grid"))
	float GridSpacing = 300.f;

	UPROPERTY(VisibleAnywhere, Category = "Mortis|Spawn", meta = (MakeEditWidget))
	TArray<FTransform> RelativeSpawnTransforms;
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Spawn")
	EMortisSpawnMode SpawnMode = EMortisSpawnMode::RandomByFloor;
	
	UPROPERTY(VisibleAnywhere, Category = "Mortis|Spawn")
	TArray<TSubclassOf<AMortisEnemyCharacter>> EnemiesToSpawn;
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Spawn", meta = (EditCondition = "SpawnMode == EMortisSpawnMode::FixedCustom"))
	FDataTableRowHandle CustomEnemySpawnRowHandle;
	
	void GeneratePoint();
	void GenerateCircle();
	void GenerateGrid();
	
private:	
	UFUNCTION()
	void OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// bool bHasSpawned = false;
	
#if WITH_EDITOR
	void DrawDebugPoints();
#endif
};
