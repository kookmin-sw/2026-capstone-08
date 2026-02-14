// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/MortisEnemyCharacter.h"

#include "MortisDebugHelper.h"
#include "AbilitySystem/Data/MortisAbilitySetBase.h"
#include "Character/Enemy/MortisEnemyData.h"
#include "Components/CapsuleComponent.h"
#include "Controllers/MortisAIController.h"

#include "GameFramework/CharacterMovementComponent.h"

AMortisEnemyCharacter::AMortisEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AMortisEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeEnemyByData();
}

void AMortisEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
}

void AMortisEnemyCharacter::InitializeEnemyByData()
{
	MORTIS_LOG("");
	if (!EnemyData)
	{
		return;
	}
	
	GetCapsuleComponent()->InitCapsuleSize(EnemyData->CapsuleRadius, EnemyData->CapsuleHalfHeight);
	GetMesh()->SetSkeletalMesh(EnemyData->EnemyMesh);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -EnemyData->CapsuleHalfHeight));

	GetCharacterMovement()->RotationRate = EnemyData->RotationRate;
	GetCharacterMovement()->MaxWalkSpeed = EnemyData->MaxWalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = EnemyData->BrakingDecelerationWalking;
	
	// if (EnemyData->AbilitySet)
	// {
	// 	EnemyData->AbilitySet->GiveToAbilitySystemComponent(MortisAbilitySystemComponent);
	// }
}

UMortisEnemyData* AMortisEnemyCharacter::GetEnemyData() const
{
	return EnemyData;
}

#if WITH_EDITOR
void AMortisEnemyCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.GetPropertyName();
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, EnemyData))
	{
		if (EnemyData)
		{
			GetCapsuleComponent()->InitCapsuleSize(EnemyData->CapsuleRadius, EnemyData->CapsuleHalfHeight);
			GetMesh()->SetSkeletalMesh(EnemyData->EnemyMesh);
			GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -EnemyData->CapsuleHalfHeight));
		}
	}
}
#endif

