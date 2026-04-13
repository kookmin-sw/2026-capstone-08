// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Pickup/MortisPickupBase.h"
#include "Components/StaticMeshComponent.h"

AMortisPickupBase::AMortisPickupBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	SelectionIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionIndicatorMesh"));
	SelectionIndicatorMesh->SetupAttachment(RootComponent);
	SelectionIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectionIndicatorMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SelectionIndicatorMesh->SetGenerateOverlapEvents(false);
	SelectionIndicatorMesh->SetCastShadow(false);
	SelectionIndicatorMesh->SetVisibility(false);
	SelectionIndicatorMesh->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
}

void AMortisPickupBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bArcMoving)
		return;

	ArcElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(ArcElapsed / FMath::Max(ArcDuration, KINDA_SMALL_NUMBER), 0.f, 1.f);

	FVector NewLocation = FMath::Lerp(ArcStart, ArcTarget, Alpha);
	NewLocation.Z += FMath::Sin(Alpha * PI) * ArcHeight;

	SetActorLocation(NewLocation);

	if (Alpha >= 1.f)
		FinishArcMove();
}

void AMortisPickupBase::ReactivateAsDroppedItem()
{
	EnableInteraction();
}

void AMortisPickupBase::StartArcMove(const FVector& InStartLocation, const FVector& InTargetLocation, float InDuration, float InArcHeight)
{
	DisableInteraction();

	ArcStart = InStartLocation;
	ArcTarget = InTargetLocation;
	ArcElapsed = 0.f;
	ArcDuration = InDuration > 0.f ? InDuration : DefaultArcDuration;
	ArcHeight = InArcHeight;

	SetActorLocation(ArcStart);

	bArcMoving = true;
	SetActorTickEnabled(true);
}

void AMortisPickupBase::SetSelectionIndicatorVisible(bool bVisible)
{
	if (SelectionIndicatorMesh)
		SelectionIndicatorMesh->SetVisibility(bVisible, true);
}

void AMortisPickupBase::OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded)
{
	if (!bSucceeded)
	{
		EnableInteraction();
		return;
	}

	DisableInteraction();
}

void AMortisPickupBase::FinishArcMove()
{
	bArcMoving = false;
	SetActorTickEnabled(false);

	SetActorLocation(ArcTarget);
	EnableInteraction();
}
