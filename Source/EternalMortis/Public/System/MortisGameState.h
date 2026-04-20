// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MortisGameState.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Mortis|State")
	int32 CurrentFloor = 1;
	
	FORCEINLINE void SetCurrentFloor(int32 NewFloor) { CurrentFloor = NewFloor; }
	FORCEINLINE int32 GetCurrentFloor() const { return CurrentFloor; }
};
