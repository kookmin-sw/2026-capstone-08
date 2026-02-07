// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MortisCharacterBase.h"

// Sets default values
AMortisCharacterBase::AMortisCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMortisCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMortisCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMortisCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

