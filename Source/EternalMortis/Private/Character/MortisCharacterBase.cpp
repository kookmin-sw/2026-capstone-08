// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MortisCharacterBase.h"

#include "MortisDebugHelper.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MortisAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Components/Combat/MortisCombatComponent.h"
#include "Items/Weapons/MortisWeaponBase.h"

// Sets default values
AMortisCharacterBase::AMortisCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;

	MortisAbilitySystemComponent = CreateDefaultSubobject<UMortisAbilitySystemComponent>(TEXT("MortisAbilitySystemComponent"));
	MortisAttributeSet = CreateDefaultSubobject<UMortisAttributeSet>(TEXT("MortisAttributeSet"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
}

UAbilitySystemComponent* AMortisCharacterBase::GetAbilitySystemComponent() const
{
	return GetMortisAbilitySystemComponent();
}

void AMortisCharacterBase::StartDeath()
{
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}

void AMortisCharacterBase::FinishDeath()
{
	if (UMortisCombatComponent* CombatComponent = GetCombatComponent())
	{
		CombatComponent->ClearWeapons(3.f);
	}
	
	if (GetMesh())
	{
		GetMesh()->bPauseAnims = true;
	}
	SetLifeSpan(3.f);
}

void AMortisCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (MortisAbilitySystemComponent)
	{
		MortisAbilitySystemComponent->InitAbilityActorInfo(this, this);

		ensureMsgf(!CharacterAbilitySet.IsNull(), TEXT("Forgot to assign start up data to %s"), *GetName());
	}
}



