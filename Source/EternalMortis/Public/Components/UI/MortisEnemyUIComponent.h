// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/MortisUIComponent.h"
#include "MortisEnemyUIComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class ETERNALMORTIS_API UMortisEnemyUIComponent : public UMortisUIComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Mortis|UI")
	bool GetCurrentHealthSnapshot(float& OutCurrentHealth, float& OutMaxHealth) const;
};
