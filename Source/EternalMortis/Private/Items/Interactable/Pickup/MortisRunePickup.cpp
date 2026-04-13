// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Pickup/MortisRunePickup.h"

#include "Components/WidgetComponent.h"
#include "System/MortisRuneDatabaseSubsystem.h"
#include "UI/MortisPickupPreviewWidget.h"

void AMortisRunePickup::SetRuneData(const FMortisRuneInstance& InRuneData)
{
	RuneData = InRuneData;

	if (SelectionWidget && SelectionWidget->IsVisible())
	{
		RefreshPickupPreviewWidget();
	}
}

void AMortisRunePickup::ReactivateAsDroppedRune(const FMortisRuneInstance& InOldRuneData, const FVector& InStartLocation, const FVector& InTargetLocation, float InDuration, float InArcHeight)
{
	SetRuneData(InOldRuneData);

	const float Duration = InDuration > 0.f ? InDuration : DefaultArcDuration;
	const float Height = InArcHeight > 0.f ? InArcHeight : DefaultArcHeight;

	StartArcMove(InStartLocation, InTargetLocation, Duration, Height);
}

bool AMortisRunePickup::BuildPickupPreviewData(FMortisPickupPreviewData& OutPreviewData) const
{
	const UGameInstance* GameInstance = GetGameInstance();
	const UMortisRuneDatabaseSubsystem* RuneDatabase = GameInstance ? GameInstance->GetSubsystem<UMortisRuneDatabaseSubsystem>() : nullptr;

	if (RuneDatabase)
	{
		OutPreviewData.TitleText = RuneDatabase->GetRuneSetDisplayName(RuneData.SetTag);
	}
	else if (RuneData.SetTag.IsValid())
	{
		OutPreviewData.TitleText = FText::FromName(RuneData.SetTag.GetTagName());
	}

	switch (RuneData.Grade)
	{
	case EMortisRuneGrade::Rare:
		OutPreviewData.AccentColor = FLinearColor(0.38f, 0.64f, 0.98f, 1.f);
		break;
	case EMortisRuneGrade::Epic:
		OutPreviewData.AccentColor = FLinearColor(0.76f, 0.47f, 0.98f, 1.f);
		break;
	case EMortisRuneGrade::Legendary:
		OutPreviewData.AccentColor = FLinearColor(0.95f, 0.72f, 0.25f, 1.f);
		break;
	default:
		OutPreviewData.AccentColor = FLinearColor(0.77f, 0.77f, 0.81f, 1.f);
		break;
	}

	return !OutPreviewData.TitleText.IsEmpty();
}
