// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickUp/MortisPickUpBase.h"

// Sets default values
AMortisPickUpBase::AMortisPickUpBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMortisPickUpBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMortisPickUpBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

