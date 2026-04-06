// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Pickup/MortisWeaponPickup.h"

void AMortisWeaponPickup::SetWeaponData(const FMortisWeaponRow& InWeaponData)
{
	WeaponData = InWeaponData;
}

void AMortisWeaponPickup::ReactivateAsDroppedWeapon(const FMortisWeaponRow& InOldWeaponData, const FVector& InStartLocation, const FVector& InTargetLocation, float InDuration, float InArcHeight)
{
	SetWeaponData(InOldWeaponData);

	const float Duration = InDuration > 0.f ? InDuration : DefaultArcDuration;
	const float Height = InArcHeight > 0.f ? InArcHeight : DefaultArcHeight;

	StartArcMove(InStartLocation, InTargetLocation, Duration, Height);
}
