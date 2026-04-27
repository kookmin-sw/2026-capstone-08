// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Pickup/MortisWeaponPickup.h"

#include "System/MortisWeaponDatabaseSubsystem.h"
#include "UI/MortisPickupPreviewWidget.h"

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

bool AMortisWeaponPickup::BuildPickupPreviewData(FMortisPickupPreviewData& OutPreviewData) const
{
	OutPreviewData.TitleText = WeaponData.WeaponName;
	if (OutPreviewData.TitleText.IsEmpty() && WeaponData.WeaponTag.IsValid())
	{
		OutPreviewData.TitleText = FText::FromName(WeaponData.WeaponTag.GetTagName());
	}
	
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (const UMortisWeaponDatabaseSubsystem* WeaponDatabase = GameInstance->GetSubsystem<UMortisWeaponDatabaseSubsystem>())
		{
			FMortisWeaponGradeStyleRow GradeStyleRow;
			if (WeaponDatabase->GetWeaponGradeStyleByHandle(WeaponData.GradeStyleRow, GradeStyleRow))
			{
				OutPreviewData.AccentColor = GradeStyleRow.GradeColor;
			}
		}
	}	

	return !OutPreviewData.TitleText.IsEmpty();
}
