// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/MortisEnemyCharacter.h"
#include "MortisDebugHelper.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "AbilitySystem/Data/MortisEnemyAbilitySet.h"
#include "Character/Enemy/MortisEnemyData.h"
#include "Components/Combat/MortisEnemyCombatComponent.h"
#include "Components/UI/MortisEnemyUIComponent.h"
#include "AbilitySystem/Attributes/MortisEnemyAttributeSet.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Controllers/MortisAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/MortisEnemyHealthBarWidget.h"
#include "UI/MortisWidgetBase.h"
#include "UObject/ConstructorHelpers.h"

AMortisEnemyCharacter::AMortisEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMortisEnemyAttributeSet>(TEXT("MortisAttributeSet")))
{
	static ConstructorHelpers::FClassFinder<UUserWidget> EnemyHealthBarWidgetClass(TEXT("/Game/UI/EnemyHUD/WBP_EnemyHPBar"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	EnemyCombatComponent = CreateDefaultSubobject<UMortisEnemyCombatComponent>("EnemyCombatComponent");
	EnemyUIComponent = CreateDefaultSubobject<UMortisEnemyUIComponent>(TEXT("EnemyUIComponent"));

	EnemyHealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHealthBarWidget"));
	EnemyHealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	EnemyHealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	EnemyHealthBarWidgetComponent->SetDrawAtDesiredSize(true);
	EnemyHealthBarWidgetComponent->SetGenerateOverlapEvents(false);
	EnemyHealthBarWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnemyHealthBarWidgetComponent->SetPivot(FVector2D(0.5f, 1.f));
	EnemyHealthBarWidgetComponent->SetVisibility(true);

	if (EnemyHealthBarWidgetClass.Succeeded())
	{
		EnemyHealthBarWidgetComponent->SetWidgetClass(EnemyHealthBarWidgetClass.Class);
	}

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AMortisEnemyCharacter::InitializeEnemyCharacter()
{
	InitializeEnemyByData();
	RegisterStateTagEvent();
	InitializeEnemyHUD();
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

void AMortisEnemyCharacter::StartDeath()
{
	Super::StartDeath();
	
	if (AMortisAIController* AIC = Cast<AMortisAIController>(GetController()))
	{
		AIC->OnEnemyDead();
	}
	SetEnemyHealthBarCombatVisibility(false);
	// GetMesh()->SetSimulatePhysics(true);
}

void AMortisEnemyCharacter::FinishDeath()
{
	Super::FinishDeath();
}

void AMortisEnemyCharacter::InitializeEnemyHUD()
{
	UpdateEnemyHealthBarWidgetLocation();

	if (!EnemyHealthBarWidgetComponent)
	{
		return;
	}

	EnemyHealthBarWidgetComponent->InitWidget();
	EnemyHealthBarWidgetComponent->SetVisibility(true);

	UUserWidget* UserWidget = EnemyHealthBarWidgetComponent->GetUserWidgetObject();
	if (!UserWidget || !EnemyUIComponent)
	{
		return;
	}

	if (UMortisWidgetBase* MortisWidget = Cast<UMortisWidgetBase>(UserWidget))
	{
		MortisWidget->BP_BindUIComponent(EnemyUIComponent);
	}

	// Keep the legacy binding path alive until the enemy HP bar blueprints are reparented.
	if (UMortisEnemyHealthBarWidget* LegacyHealthBarWidget = Cast<UMortisEnemyHealthBarWidget>(UserWidget))
	{
		LegacyHealthBarWidget->InitializeFromUIComponent(EnemyUIComponent);
	}
}

void AMortisEnemyCharacter::InitializeEnemyByData()
{
	if (!ensureMsgf(EnemyData, TEXT("%s: Enemy Data is not set"), *GetName()))
	{
		return;
	}

	check(GetCapsuleComponent() && GetMesh() && GetCharacterMovement());
	
	GetCapsuleComponent()->InitCapsuleSize(EnemyData->CapsuleRadius, EnemyData->CapsuleHalfHeight);
	
	USkeletalMesh* SelectedMesh = EnemyData->EnemyMesh;
	if (EnemyData->bEnableVisualVariations && !EnemyData->MeshPool.IsEmpty())
	{
		int32 RandomIndex = FMath::RandRange(0, EnemyData->MeshPool.Num() - 1);
		if (EnemyData->MeshPool[RandomIndex])
		{
			SelectedMesh = EnemyData->MeshPool[RandomIndex];
		}
	}

	GetMesh()->SetSkeletalMesh(SelectedMesh);
	
	if (EnemyData->bEnableVisualVariations && !EnemyData->MaterialSetPool.IsEmpty())
	{
		int32 RandomIndex = FMath::RandRange(0, EnemyData->MaterialSetPool.Num() - 1);
		ApplyMaterialSet(EnemyData->MaterialSetPool[RandomIndex]);
	}
	
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

	UpdateEnemyHealthBarWidgetLocation();
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

void AMortisEnemyCharacter::SetEnemyHealthBarCombatVisibility(bool bShouldShow)
{
	(void)bShouldShow;

	// Deprecated: enemy HUD visibility is now owned by the widget blueprint.
}

void AMortisEnemyCharacter::UpdateEnemyHealthBarWidgetLocation()
{
	if (!EnemyHealthBarWidgetComponent || !GetCapsuleComponent())
	{
		return;
	}

	const float CapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	EnemyHealthBarWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, CapsuleHalfHeight + EnemyHealthBarHeightOffset));
}

void AMortisEnemyCharacter::ApplyMaterialSet(const FMortisMaterialSet& MaterialSet)
{
	for (int32 i = 0; i < MaterialSet.Materials.Num(); i++)
	{
		if (UMaterialInterface* Material = MaterialSet.Materials[i])
		{
			GetMesh()->SetMaterial(i, Material);
		}
	}
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

	UpdateEnemyHealthBarWidgetLocation();
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
