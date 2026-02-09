// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/MortisAnimInstanceBase.h"
#include "MortisCharacterAnimInstanceBase.generated.h"

class AMortisCharacterBase;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisCharacterAnimInstanceBase : public UMortisAnimInstanceBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	AMortisCharacterBase* OwningCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float LocomotionDirection;

public:
	// UAnimInstance Override
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
};
