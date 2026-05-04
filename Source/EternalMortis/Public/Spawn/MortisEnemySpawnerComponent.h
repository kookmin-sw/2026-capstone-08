// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Spawn/MortisSpawnTypes.h"
#include "MortisEnemySpawnerComponent.generated.h"


class UBoxComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ETERNALMORTIS_API UMortisEnemySpawnerComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UMortisEnemySpawnerComponent();
	
	FORCEINLINE EMortisSpawnMode GetSpawnMode() const { return SpawnMode; }
	FORCEINLINE FDataTableRowHandle GetCustomSpawnRowHandle() const { return CustomEnemySpawnRowHandle; }
	
	void SetEnemiesToSpawn(const TArray<TSubclassOf<AMortisEnemyCharacter>>& NewEnemiesToSpawn);
	
	UFUNCTION(BlueprintCallable)
	TArray<AMortisEnemyCharacter*> SpawnEnemies() const;
	
#if WITH_EDITORONLY_DATA
	virtual void OnRegister() override;
#endif
	
protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, Category = "Mortis|Spawn")
	TObjectPtr<UBillboardComponent> SpriteComponent;
#endif
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Spawn")
	EMortisSpawnMode SpawnMode = EMortisSpawnMode::RandomByFloor;
	
	UPROPERTY(VisibleAnywhere, Category = "Mortis|Spawn")
	TArray<TSubclassOf<AMortisEnemyCharacter>> EnemiesToSpawn;
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Spawn", meta = (EditCondition = "SpawnMode == EMortisSpawnMode::FixedCustom"))
	FDataTableRowHandle CustomEnemySpawnRowHandle;
};
