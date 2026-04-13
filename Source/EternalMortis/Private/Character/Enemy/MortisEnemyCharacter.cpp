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

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AMortisEnemyCharacter::InitializeEnemyCharacter()
{
	InitializeEnemyByData();
	RegisterStateTagEvent();
}

void AMortisEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMortisEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// bool A = GetCharacterMovement()->bOrientRotationToMovement;
	// bool B = GetCharacterMovement()->bUseControllerDesiredRotation;
	// bool C = bUseControllerRotationYaw;
	// FString DebugString = FString::Printf(TEXT("OrientToMovement: %s, UseControllerRotation: %s, UseControllerYaw: %s"), *Debug::ToString(A), *Debug::ToString(B), *Debug::ToString(C));
	// DrawDebugString(GetWorld(), GetActorLocation() + FVector(0.f, -50.f, 100.f), DebugString, this, FColor::Yellow, 0.f);
	//
}


// void AMortisEnemyCharacter::PossessedBy(AController* NewController)
// {
// 	Super::PossessedBy(NewController);
// }

void AMortisEnemyCharacter::InitializeEnemyByData()
{
	if (!ensureMsgf(EnemyData, TEXT("%s: Enemy Data is not set"), *GetName()))
	{
		return;
	}

	check(GetCapsuleComponent() && GetMesh() && GetCharacterMovement());
	
	GetCapsuleComponent()->InitCapsuleSize(EnemyData->CapsuleRadius, EnemyData->CapsuleHalfHeight);
	GetMesh()->SetSkeletalMesh(EnemyData->EnemyMesh);
	GetMesh()->SetRelativeScale3D(EnemyData->MeshScale);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -EnemyData->CapsuleHalfHeight));
	GetMesh()->SetRelativeRotation(EnemyData->MeshRotation);

	if (EnemyData->AnimClass)
	{
		GetMesh()->SetAnimInstanceClass(EnemyData->AnimClass);
	}

	if (EnemyData->LinkedAnimLayerClass)
	{
		GetMesh()->LinkAnimClassLayers(EnemyData->LinkedAnimLayerClass);
	}
	
	GetCharacterMovement()->RotationRate = EnemyData->RotationRate;
	GetCharacterMovement()->MaxWalkSpeed = EnemyData->IdleMaxWalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = EnemyData->BrakingDecelerationWalking;

	CharacterAbilitySet = EnemyData->AbilitySet;
	
	// is Game playing
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

float AMortisEnemyCharacter::GetRandomStrafingDistance() const
{
	if (!EnemyData || !EnemyData->PhaseStrafingRanges.Contains(CurrentPhase))
	{
		return 500.f;
	}
	return FMath::RandRange(EnemyData->PhaseStrafingRanges[CurrentPhase].MinDistance, EnemyData->PhaseStrafingRanges[CurrentPhase].MaxDistance);
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
		MORTIS_LOG("ASC is null");
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
	if (!EnemyData)
	{
		return;
	}
	bool bStrafing = NewCount > 0;
	// MORTIS_LOG("Strafing %s", NewCount > 0 ? TEXT("Begin") : TEXT("End"));
	bUseControllerRotationYaw = bStrafing;
	GetCharacterMovement()->bOrientRotationToMovement = !bStrafing;
	GetCharacterMovement()->bUseControllerDesiredRotation = bStrafing;
	GetCharacterMovement()->MaxWalkSpeed = bStrafing ? EnemyData->StrafingMaxWalkSpeed : EnemyData->IdleMaxWalkSpeed;
}

void AMortisEnemyCharacter::OnChasingStateChanged(FGameplayTag Tag, int32 NewCount) const
{
	if (!EnemyData)
	{
		return;
	}
	// MORTIS_LOG("Chasing %s", NewCount > 0 ? TEXT("Begin") : TEXT("End"));
	bool bChasing = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bChasing ? EnemyData->ChasingMaxWalkSpeed : EnemyData->IdleMaxWalkSpeed;
}
