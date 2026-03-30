// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisEnemyStaticWeapon.h"

AMortisEnemyStaticWeapon::AMortisEnemyStaticWeapon()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(WeaponRoot);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
