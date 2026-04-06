// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "MortisPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
private:
	FGenericTeamId PlayerTeamID;

public:
	AMortisPlayerController();

	// IGenericTeamAgentInterface Override.
	virtual FGenericTeamId GetGenericTeamId() const override;
};
