// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MortisWeaponInventorySubsystem.h"

void UMortisWeaponInventorySubsystem::SetCurrentWeapon(const FMortisWeaponRow& InWeaponRow)
{
	CurrentWeapon = InWeaponRow;
	OnWeaponChanged.Broadcast(CurrentWeapon);
}

void UMortisWeaponInventorySubsystem::ClearCurrentWeapon()
{
	CurrentWeapon = FMortisWeaponRow();
	OnWeaponChanged.Broadcast(CurrentWeapon);
}

void UMortisWeaponInventorySubsystem::SetInitialWeaponTagFromSave()
{
	// TODO : 세이브 파일 같은 것에서 무기 가져오기

	
}
