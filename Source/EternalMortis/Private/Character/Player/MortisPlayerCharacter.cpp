// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/MortisPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/Data/MortisAbilitySetBase.h"
#include "AbilitySystem/Attributes/MortisPlayerAttributeSet.h"
#include "EnhancedInputSubsystems.h"
#include "Components/Input/MortisInputComponent.h"
#include "Components/Combat/MortisPlayerCombatComponent.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "Components/UI/MortisPlayerUIComponent.h"
#include "MortisGameplayTags.h"

AMortisPlayerCharacter::AMortisPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMortisPlayerAttributeSet>(TEXT("MortisAttributeSet")))
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraArm->SetupAttachment(GetRootComponent());
	CameraArm->TargetArmLength = 200.0f;
	CameraArm->SocketOffset = FVector(0.0f, 55.0f, 65.0f);
	CameraArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	MortisPlayerCombatComponent = CreateDefaultSubobject<UMortisPlayerCombatComponent>("MortisPlayerCombatComponent");

	PlayerUIComponent = CreateDefaultSubobject<UMortisPlayerUIComponent>(TEXT("HeroUIComponent"));
}

void AMortisPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!CharacterAbilitySet.IsNull())
	{
		if (UMortisAbilitySetBase* LoadedData = CharacterAbilitySet.LoadSynchronous())
		{
			LoadedData->GiveToAbilitySystemComponent(MortisAbilitySystemComponent);
		}
	}
}

void AMortisPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset, TEXT("Assign Data Asset Correctly"));
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	UMortisInputComponent* MortisInputComponent = CastChecked<UMortisInputComponent>(PlayerInputComponent);
	MortisInputComponent->BindNativeInputAction(InputConfigDataAsset, MortisGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	MortisInputComponent->BindNativeInputAction(InputConfigDataAsset, MortisGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);

	MortisInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
}

void AMortisPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMortisPlayerCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (MovementVector.Y != 0.0f)
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);

		AddMovementInput(ForwardDirection, MovementVector.Y);
	}
	if (MovementVector.X != 0.0f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);

		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMortisPlayerCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	if (LookAxisVector.X != 0.0f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}
	if (LookAxisVector.Y != 0.0f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMortisPlayerCharacter::Input_AbilityInputPressed(FGameplayTag InputTag)
{
	MortisAbilitySystemComponent->OnAbilityInputPressed(InputTag);
}

void AMortisPlayerCharacter::Input_AbilityInputReleased(FGameplayTag InputTag)
{
	MortisAbilitySystemComponent->OnAbilityInputReleased(InputTag);
}
