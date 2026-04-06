// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/PointInteractable/MortisChest.h"

AMortisChest::AMortisChest()
{

}

bool AMortisChest::CanInteract(APawn* InteractingPawn) const
{
	return Super::CanInteract(InteractingPawn) && !bAlreadyOpened;
}

void AMortisChest::OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded)
{
	if (bSucceeded)
	{
		bAlreadyOpened = true;
		DisableInteraction();
	}
	else
		EnableInteraction();
}
