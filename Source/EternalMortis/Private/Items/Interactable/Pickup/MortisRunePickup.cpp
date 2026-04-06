// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Pickup/MortisRunePickup.h"

void AMortisRunePickup::SetRuneData(const FMortisRuneInstance& InRuneData)
{
	RuneData = InRuneData;
}

void AMortisRunePickup::ReactivateAsDroppedRune(const FMortisRuneInstance& InOldRuneData, const FVector& InStartLocation, const FVector& InTargetLocation, float InDuration, float InArcHeight)
{
	SetRuneData(InOldRuneData);

	const float Duration = InDuration > 0.f ? InDuration : DefaultArcDuration;
	const float Height = InArcHeight > 0.f ? InArcHeight : DefaultArcHeight;

	StartArcMove(InStartLocation, InTargetLocation, Duration, Height);
}
