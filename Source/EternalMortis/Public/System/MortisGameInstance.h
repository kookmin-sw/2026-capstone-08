// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MortisGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 현재 층 (3부터 시작 - 성 내부)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon")
	int32 CurrentFloor = 0;

	// 현재 컨셉 (0: 지하감옥, 1: 성 내부, 2: 식인마을)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon")
	int32 CurrentConcept = 0;


};
