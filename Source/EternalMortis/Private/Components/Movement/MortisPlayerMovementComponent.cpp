// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Movement/MortisPlayerMovementComponent.h"

void UMortisPlayerMovementComponent::SetTargetSpeed(float MoveSpeed)
{
	TargetSpeedCap = FMath::Max(0.0f, MoveSpeed);
}

void UMortisPlayerMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	CurrentSpeedCap = FMath::FInterpTo(CurrentSpeedCap, TargetSpeedCap, DeltaSeconds, SpeedCapInterp);
}

float UMortisPlayerMovementComponent::GetMaxSpeed() const
{
	return CurrentSpeedCap;
}
