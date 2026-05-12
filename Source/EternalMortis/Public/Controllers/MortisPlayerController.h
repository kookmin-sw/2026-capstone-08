// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "MortisPlayerController.generated.h"

class UWidgetInteractionComponent;

/**
 *
 */
UCLASS()
class ETERNALMORTIS_API AMortisPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

private:
	FGenericTeamId PlayerTeamID;

protected:
	virtual void PlayerTick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mortis|UI")
	float GamepadCursorSpeed = 1200.f;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|UI")
	FVector2D GamepadCursorInput = FVector2D::ZeroVector;

public:
	AMortisPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Mortis|UI")
	void SetGamepadCursorInput(FVector2D InInput);

	UFUNCTION(BlueprintCallable, Category = "Mortis|UI")
	bool ClickHoveredButton();

	virtual FGenericTeamId GetGenericTeamId() const override;
};
