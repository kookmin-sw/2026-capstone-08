// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/MortisEnemyCharacter.h"
#include "MortisDebugHelper.h"
#include "AbilitySystem/Data/MortisEnemyAbilitySet.h"
#include "Character/Enemy/MortisEnemyData.h"
#include "Components/Combat/MortisEnemyCombatComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AMortisEnemyCharacter::AMortisEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	EnemyCombatComponent = CreateDefaultSubobject<UMortisEnemyCombatComponent>("EnemyCombatComponent");
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
		MORTIS_LOG("Enemy data is null");
		return;
	}
	
	GetCapsuleComponent()->InitCapsuleSize(EnemyData->CapsuleRadius, EnemyData->CapsuleHalfHeight);
	GetMesh()->SetSkeletalMesh(EnemyData->EnemyMesh);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -EnemyData->CapsuleHalfHeight));

	GetCharacterMovement()->RotationRate = EnemyData->RotationRate;
	GetCharacterMovement()->MaxWalkSpeed = EnemyData->MaxWalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = EnemyData->BrakingDecelerationWalking;


	//
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		if (EnemyData->AbilitySet)
		{
			EnemyData->AbilitySet->GiveToAbilitySystemComponent(MortisAbilitySystemComponent);
		}

		if (EnemyData->AttackPatternData)
		{
			if (EnemyCombatComponent)
			{
				EnemyCombatComponent->SetAttackPattern(EnemyData->AttackPatternData);
			}
		}
	}
}

UMortisEnemyData* AMortisEnemyCharacter::GetEnemyData() const
{
	return EnemyData;
}

UMortisCombatComponent* AMortisEnemyCharacter::GetCombatComponent() const
{
	return EnemyCombatComponent;
}

UMortisEnemyCombatComponent* AMortisEnemyCharacter::GetEnemyCombatComponent() const
{
	return EnemyCombatComponent;
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
			InitializeEnemyByData();
		}
	}
}
#endif

