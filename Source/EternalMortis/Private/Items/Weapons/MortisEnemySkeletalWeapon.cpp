// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisEnemySkeletalWeapon.h"

AMortisEnemySkeletalWeapon::AMortisEnemySkeletalWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(WeaponRoot);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
