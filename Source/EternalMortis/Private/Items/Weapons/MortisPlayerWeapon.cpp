// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisPlayerWeapon.h"

void AMortisPlayerWeapon::AssignWeaponAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle> SpecHandelsToGrant)
{
	GrantedAbilitySpecHandles = SpecHandelsToGrant;
}

TArray<FGameplayAbilitySpecHandle> AMortisPlayerWeapon::GetGrantedAbilitySpecHandels() const
{
	return GrantedAbilitySpecHandles;
}
