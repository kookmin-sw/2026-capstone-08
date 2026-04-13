// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Gate/MortisBossGate.h"

AMortisBossGate::AMortisBossGate()
{
}

bool AMortisBossGate::CanInteract(APawn* InteractingPawn) const
{
	return Super::CanInteract(InteractingPawn) && !bAlreadyPassed;
}

void AMortisBossGate::OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded)
{
	Super::OnInteractionFinished(InteractingPawn, bSucceeded);

	if (bSucceeded)
	{
		bAlreadyPassed = true;
		DisableInteraction();
	}
}
