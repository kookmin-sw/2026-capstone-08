// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Pickup/MortisPickupBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/MortisPickupPreviewWidget.h"

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
	
	SelectionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("SelectionWidget"));
	SelectionWidget->SetupAttachment(RootComponent);
	SelectionWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	SelectionWidget->SetWidgetClass(UMortisPickupPreviewWidget::StaticClass());
	SelectionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	SelectionWidget->SetDrawAtDesiredSize(true);
	SelectionWidget->SetGenerateOverlapEvents(false);
	SelectionWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectionWidget->SetPivot(FVector2D(0.5f, 1.f));
	SelectionWidget->SetVisibility(false);
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
	
	if (SelectionWidget)
	{
		if (bVisible)
		{
			RefreshPickupPreviewWidget();
		}
		else if (UMortisPickupPreviewWidget* PickupPreviewWidget = GetPickupPreviewWidget())
		{
			PickupPreviewWidget->ClearPreview();
		}

		SelectionWidget->SetVisibility(bVisible);
	}
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

bool AMortisPickupBase::BuildPickupPreviewData(FMortisPickupPreviewData& OutPreviewData) const
{
	return false;
}

void AMortisPickupBase::RefreshPickupPreviewWidget()
{
	if (UMortisPickupPreviewWidget* PickupPreviewWidget = GetPickupPreviewWidget())
	{
		FMortisPickupPreviewData PreviewData;
		if (BuildPickupPreviewData(PreviewData))
		{
			PickupPreviewWidget->ApplyPreviewData(PreviewData);
		}
		else
		{
			PickupPreviewWidget->ClearPreview();
		}
	}
}

UMortisPickupPreviewWidget* AMortisPickupBase::GetPickupPreviewWidget() const
{
	if (!SelectionWidget)
	{
		return nullptr;
	}

	SelectionWidget->InitWidget();
	return Cast<UMortisPickupPreviewWidget>(SelectionWidget->GetUserWidgetObject());
}

void AMortisPickupBase::FinishArcMove()
{
	bArcMoving = false;
	SetActorTickEnabled(false);

	SetActorLocation(ArcTarget);
	EnableInteraction();
}
