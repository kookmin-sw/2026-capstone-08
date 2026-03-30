// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisPlayerWeapon.h"

AMortisPlayerWeapon::AMortisPlayerWeapon()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(WeaponRoot);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMortisPlayerWeapon::AssignWeaponAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle> SpecHandlesToGrant)
{
	GrantedAbilitySpecHandles = SpecHandlesToGrant;
}

TArray<FGameplayAbilitySpecHandle> AMortisPlayerWeapon::GetGrantedAbilitySpecHandles() const
{
	return GrantedAbilitySpecHandles;
}
