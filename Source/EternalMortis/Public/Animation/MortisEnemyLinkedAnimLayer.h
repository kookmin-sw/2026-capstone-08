// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/MortisAnimInstanceBase.h"
#include "MortisEnemyLinkedAnimLayer.generated.h"

class UMortisEnemyAnimInstance;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisEnemyLinkedAnimLayer : public UMortisAnimInstanceBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UMortisEnemyAnimInstance* GetEnemyAnimInstance() const;
};
