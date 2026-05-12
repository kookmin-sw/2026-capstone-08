// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Pickup/SecretHelper/MortisSecretHelperRunePickup.h"
#include "Character/SecretHelper/MortisSecretHelper.h"

void AMortisSecretHelperRunePickup::SetOwningSecretHelper(AMortisSecretHelper* InHelper)
{
	OwningSecretHelper = InHelper;
}

void AMortisSecretHelperRunePickup::OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded)
{
	Super::OnInteractionFinished(InteractingPawn, bSucceeded);

	if (!bSucceeded)
		return;

	if (IsValid(OwningSecretHelper))
		OwningSecretHelper->NotifyOwnedPickupInteracted(this, InteractingPawn);
}
