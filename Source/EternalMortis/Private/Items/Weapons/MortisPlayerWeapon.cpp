// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisPlayerWeapon.h"

void AMortisPlayerWeapon::AssignWeaponAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle> SpecHandlesToGrant)
{
	GrantedAbilitySpecHandles = SpecHandlesToGrant;
}

TArray<FGameplayAbilitySpecHandle> AMortisPlayerWeapon::GetGrantedAbilitySpecHandles() const
{
	return GrantedAbilitySpecHandles;
}
