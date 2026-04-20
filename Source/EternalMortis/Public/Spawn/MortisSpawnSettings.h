// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MortisSpawnSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Enemy Spawn Settings"))
class ETERNALMORTIS_API UMortisSpawnSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Spawning")
	TMap<int32, FDataTableRowHandle> FloorSpawnPools;
};
