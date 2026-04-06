// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/PointInteractable/MortisPointInteractableActorBase.h"

AMortisPointInteractableActorBase::AMortisPointInteractableActorBase()
{
	InteractionPoint = CreateDefaultSubobject<USceneComponent>(TEXT("InteractionPoint"));
	InteractionPoint->SetupAttachment(Root);
}

USceneComponent* AMortisPointInteractableActorBase::GetInteractionPoint() const
{
	return InteractionPoint;
}
