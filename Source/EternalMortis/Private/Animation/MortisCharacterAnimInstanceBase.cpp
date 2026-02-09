// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MortisCharacterAnimInstanceBase.h"
#include "Character/MortisCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

void UMortisCharacterAnimInstanceBase::NativeInitializeAnimation()
{
	OwningCharacter = Cast<AMortisCharacterBase>(TryGetPawnOwner());
	if (OwningCharacter)
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
}

void UMortisCharacterAnimInstanceBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	UE_LOG(LogTemp, Display, TEXT("OwningCharacter : %d | OwningMovementComponent : %d"), OwningCharacter, OwningMovementComponent);
	if (!OwningCharacter || !OwningMovementComponent) return;

	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0;
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
}
