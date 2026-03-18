// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/MortisEnemyCharacter.h"
#include "MortisDebugHelper.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "AbilitySystem/Data/MortisEnemyAbilitySet.h"
#include "Character/Enemy/MortisEnemyData.h"
#include "Components/Combat/MortisEnemyCombatComponent.h"
#include "AbilitySystem/Attributes/MortisEnemyAttributeSet.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AMortisEnemyCharacter::AMortisEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMortisEnemyAttributeSet>(TEXT("MortisAttributeSet")))
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
	RegisterStateTagEvent();
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

	if (!GetCapsuleComponent() || !GetMesh() || !GetCharacterMovement())
	{
		return;
	}
	
	GetCapsuleComponent()->InitCapsuleSize(EnemyData->CapsuleRadius, EnemyData->CapsuleHalfHeight);
	GetMesh()->SetSkeletalMesh(EnemyData->EnemyMesh);
	GetMesh()->SetRelativeScale3D(EnemyData->MeshScale);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -EnemyData->CapsuleHalfHeight));
	GetMesh()->SetRelativeRotation(EnemyData->MeshRotation);
	GetMesh()->SetAnimInstanceClass(EnemyData->AnimClass);
	
	GetCharacterMovement()->RotationRate = EnemyData->RotationRate;
	GetCharacterMovement()->MaxWalkSpeed = EnemyData->MaxWalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = EnemyData->BrakingDecelerationWalking;

	CharacterAbilitySet = EnemyData->AbilitySet;
	//
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		IdleMaxWalkSpeed = EnemyData->IdleMaxWalkSpeed;
		StrafingMaxWalkSpeed = EnemyData->StrafingMaxWalkSpeed;
		ChasingMaxWalkSpeed = EnemyData->ChasingMaxWalkSpeed;
		
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

void AMortisEnemyCharacter::RegisterStateTagEvent()
{
	if (!MortisAbilitySystemComponent)
	{
		return;
	}
	
	MortisAbilitySystemComponent->RegisterGameplayTagEvent(
		MortisGameplayTags::State_Movement_Strafing,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &ThisClass::OnStrafingStateChanged);

	MortisAbilitySystemComponent->RegisterGameplayTagEvent(
		MortisGameplayTags::State_Movement_Chasing,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &ThisClass::OnChasingStateChanged);
}

void AMortisEnemyCharacter::OnStrafingStateChanged(const FGameplayTag Tag, int32 NewCount)
{
	// MORTIS_LOG("Strafing %s", NewCount > 0 ? TEXT("Begin") : TEXT("End"));
	bool bStrafing = NewCount > 0;
	bUseControllerRotationYaw = bStrafing;
	GetCharacterMovement()->bOrientRotationToMovement = !bStrafing;
	GetCharacterMovement()->bUseControllerDesiredRotation = bStrafing;
	GetCharacterMovement()->MaxWalkSpeed = bStrafing ? StrafingMaxWalkSpeed : IdleMaxWalkSpeed;
}

void AMortisEnemyCharacter::OnChasingStateChanged(FGameplayTag Tag, int32 NewCount) const
{
	// MORTIS_LOG("Chasing %s", NewCount > 0 ? TEXT("Begin") : TEXT("End"));
	bool bChasing = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bChasing ? ChasingMaxWalkSpeed : IdleMaxWalkSpeed;
}
