// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisWeaponBase.h"

// Sets default values
AMortisWeaponBase::AMortisWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMortisWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMortisWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

