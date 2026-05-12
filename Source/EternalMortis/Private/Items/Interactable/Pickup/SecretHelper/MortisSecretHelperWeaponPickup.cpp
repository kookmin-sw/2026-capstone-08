// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Pickup/SecretHelper/MortisSecretHelperWeaponPickup.h"
#include "Character/SecretHelper/MortisSecretHelper.h"

void AMortisSecretHelperWeaponPickup::SetOwningSecretHelper(AMortisSecretHelper* InHelper)
{
	OwningSecretHelper = InHelper;
}

void AMortisSecretHelperWeaponPickup::OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded)
{
	Super::OnInteractionFinished(InteractingPawn, bSucceeded);

	if (!bSucceeded)
		return;

	if (IsValid(OwningSecretHelper))
		OwningSecretHelper->NotifyOwnedPickupInteracted(this, InteractingPawn);
}