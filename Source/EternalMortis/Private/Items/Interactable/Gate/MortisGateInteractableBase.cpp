// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Gate/MortisGateInteractableBase.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"

AMortisGateInteractableBase::AMortisGateInteractableBase()
{
	FrontInteractZone = CreateDefaultSubobject<UBoxComponent>(TEXT("FrontInteractZone"));
	FrontInteractZone->SetupAttachment(Root);
	FrontInteractZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FrontInteractZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	FrontInteractZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	FrontInteractZone->SetGenerateOverlapEvents(true);

	GateBlockCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("GateBlockCollision"));
	GateBlockCollision->SetupAttachment(Root);
	GateBlockCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GateBlockCollision->SetCollisionResponseToAllChannels(ECR_Block);

	PassEndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PassEndPoint"));
	PassEndPoint->SetupAttachment(Root);
}

bool AMortisGateInteractableBase::CanInteract(APawn* InteractingPawn) const
{
	if (!Super::CanInteract(InteractingPawn) || !InteractingPawn)
		return false;

	if (bRequireFrontZone && !IsPawnInFrontZone(InteractingPawn))
		return false;

	if (bOneWayOnly && !IsPawnOnAllowedSide(InteractingPawn))
		return false;

	return true;
}

bool AMortisGateInteractableBase::IsPawnInFrontZone(APawn* InteractingPawn) const
{
	if (!FrontInteractZone || !InteractingPawn)
		return false;

	return FrontInteractZone->IsOverlappingActor(InteractingPawn);
}

bool AMortisGateInteractableBase::IsPawnOnAllowedSide(APawn* InteractingPawn) const
{
	if (!InteractingPawn)
		return false;

	const FVector GateForward = GetActorForwardVector();
	const FVector ToGate = (GetActorLocation() - InteractingPawn->GetActorLocation()).GetSafeNormal();
	const float Dot = FVector::DotProduct(GateForward, ToGate);

	return Dot >= RequiredFrontDot;
}
