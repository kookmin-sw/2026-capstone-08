// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MortisPlayerMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	// 걷기 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mortis|Acceleration")
	float WalkSpeed = 375.f;

	// 달리기 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mortis|Acceleration")
	float SprintSpeed = 600.f;

	// 작을수록 천천히 내려감
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mortis|Acceleration")
	float SpeedCapInterp = 6.f;

	UFUNCTION(BlueprintCallable)
	void SetSprinting(bool bSprinting);

protected:
	float CurrentSpeedCap = 375.f;
	float TargetSpeedCap = 375.f;

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual float GetMaxSpeed() const override;
};
