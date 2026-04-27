// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/MortisPlayerCharacter.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/Data/MortisAbilitySetBase.h"
#include "AbilitySystem/Attributes/MortisPlayerAttributeSet.h"
#include "EnhancedInputSubsystems.h"
#include "Components/Input/MortisInputComponent.h"
#include "Components/Combat/MortisPlayerCombatComponent.h"
#include "Components/Equipment/MortisEquipmentComponent.h"
#include "Components/UI/MortisPlayerUIComponent.h"
#include "Components/Movement/MortisPlayerMovementComponent.h"
#include "Components/Collisions/MortisInteractionComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "MotionWarpingComponent.h"
#include "MortisGameplayTags.h"

#include "MortisDebugHelper.h"

AMortisPlayerCharacter::AMortisPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
		.SetDefaultSubobjectClass<UMortisPlayerMovementComponent>(ACharacter::CharacterMovementComponentName)
		.SetDefaultSubobjectClass<UMortisPlayerAttributeSet>(TEXT("MortisAttributeSet")))
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraArm->SetupAttachment(GetRootComponent());
	CameraArm->TargetArmLength = TargetZoomLength;
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

	EquipmentComponent = CreateDefaultSubobject<UMortisEquipmentComponent>(TEXT("EquipmentComponent"));

	InteractionComponent = CreateDefaultSubobject<UMortisInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->SetupAttachment(GetRootComponent());
	InteractionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionComponent->SetCollisionObjectType(ECC_Pawn);
	InteractionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	InteractionComponent->SetGenerateOverlapEvents(true);
	InteractionComponent->SetSphereRadius(200.f);
	InteractionComponent->SetRelativeLocation(FVector(80.f, 0.f, 60.f));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	MortisAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UMortisPlayerAttributeSet::GetMoveSpeedAttribute()).AddUObject(this, &AMortisPlayerCharacter::ChangeMovementMaxSpeed);
}

void AMortisPlayerCharacter::SetRecoveryMontage(UAnimMontage* InMontage)
{
	CurrentRecoveryMontage = InMontage;
}

void AMortisPlayerCharacter::StopRecoveryMontage(float BlendOutTime)
{
	if (!CurrentRecoveryMontage) return;

	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		if (Anim->Montage_IsPlaying(CurrentRecoveryMontage))
			Anim->Montage_Stop(BlendOutTime, CurrentRecoveryMontage);
	}

	CurrentRecoveryMontage = nullptr;
}

void AMortisPlayerCharacter::ExecuteBufferedAbility()
{
	if (!BufferedInputTag.IsValid()) return;

	MortisAbilitySystemComponent->OnAbilityInputPressed(BufferedInputTag);
	BufferedInputTag = FGameplayTag::EmptyTag;
}

UMortisCombatComponent* AMortisPlayerCharacter::GetCombatComponent() const
{
	return MortisPlayerCombatComponent;
}

void AMortisPlayerCharacter::SetAllInputEnabled(bool bMoveEnabled, bool bLookEnabled, bool bAbilityEnabled)
{
	bCanMoveInput = bMoveEnabled;
	bCanLookInput = bLookEnabled;
	bCanAbilityInput = bAbilityEnabled;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetIgnoreMoveInput(!bCanMoveInput);
		PC->SetIgnoreLookInput(!bCanLookInput);
	}
}

void AMortisPlayerCharacter::SetMoveInputByAbility(bool bMoveEnabled)
{
	bCanMoveInputByAbility = bMoveEnabled;
}

void AMortisPlayerCharacter::UpdateAttackDirectionWarpTarget(const FRotator& InRotation)
{
	if (!MotionWarpingComponent)
		return;

	const FTransform WarpTargetTransform(
		FRotator(0.f, InRotation.Yaw, 0.f),
		GetActorLocation()
	);

	MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform(
		TEXT("AttackDirection"),
		WarpTargetTransform
	);
}

void AMortisPlayerCharacter::SetLockOnZoomAlpha(float InAlpha)
{
	const float ClampedAlpha = FMath::Clamp(InAlpha, 0.f, 1.f);
	TargetZoomLength = FMath::Lerp(MinZoomLength, MaxZoomLength, ClampedAlpha);
}

void AMortisPlayerCharacter::SetLockOnHeightOffsetEnabled(bool bEnabled)
{
	bLockOnHeightOffsetEnabled = bEnabled;
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
	MortisInputComponent->BindNativeInputAction(InputConfigDataAsset, MortisGameplayTags::InputTag_Zoom, ETriggerEvent::Triggered, this, &ThisClass::Input_Zoom);
	MortisInputComponent->BindNativeInputAction(InputConfigDataAsset, MortisGameplayTags::InputTag_TargetChange, ETriggerEvent::Started, this, &ThisClass::Input_TargetChange);

	MortisInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
}

void AMortisPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AMortisPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// FString DebugString = MortisPlayerCombatComponent->CurrentEquippedWeaponTag.ToString(); 
	// DrawDebugString(GetWorld(), GetActorLocation() + FVector(0.f, 0.f, 100.f), DebugString, this, FColor::Yellow, 0.f);
	
	CameraArm->TargetArmLength = FMath::FInterpTo(
		CameraArm->TargetArmLength,
		TargetZoomLength,
		DeltaTime,
		ZoomInterpSpeed
	);

	const float ZoomAlpha = FMath::GetMappedRangeValueClamped(
		FVector2D(MinZoomLength, MaxZoomLength),
		FVector2D(0.f, 1.f),
		CameraArm->TargetArmLength
	);

	const float TargetHeightOffset = FMath::Lerp(ZoomInHeightOffset, ZoomOutHeightOffset, ZoomAlpha) + (bLockOnHeightOffsetEnabled ? LockOnHeightOffset : 0.f);

	FVector SocketOffset = CameraArm->SocketOffset;
	SocketOffset.Z = FMath::FInterpTo(
		SocketOffset.Z,
		TargetHeightOffset,
		DeltaTime,
		ZoomHeightInterpSpeed
	);

	CameraArm->SocketOffset = SocketOffset;
}

void AMortisPlayerCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	if (!bCanMoveInput || !bCanMoveInputByAbility) return;

	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (MovementVector.SizeSquared() > 0.01f) { 
		StopRecoveryMontage(RecoveryBlendOutTime);

		const FVector WorldMoveDirection = (MovementRotation.RotateVector(FVector(MovementVector.Y, MovementVector.X, 0.0f))).GetSafeNormal();
		if (!WorldMoveDirection.IsNearlyZero())
		{
			const FRotator AttackDirectionRotation = WorldMoveDirection.Rotation();
			const FTransform WarpTargetTransform(AttackDirectionRotation, GetActorLocation());

			MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform(TEXT("AttackDirection"),WarpTargetTransform);
		}
	}

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
	if (!bCanLookInput) return;

	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	{
		AddControllerYawInput(LookAxisVector.X);
	}
	if (LookAxisVector.Y != 0.0f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMortisPlayerCharacter::Input_Zoom(const FInputActionValue& InputActionValue)
{
	if (!bCanLookInput) return;

	const float AxisValue = InputActionValue.Get<float>();
	if (FMath::IsNearlyZero(AxisValue)) return;

	TargetZoomLength = FMath::Clamp(
		TargetZoomLength - AxisValue * ZoomStep,
		MinZoomLength,
		MaxZoomLength
	);
}

void AMortisPlayerCharacter::Input_TargetChange(const FInputActionValue& InputActionValue)
{
	if (!bCanAbilityInput) return;

	const float AxisValue = InputActionValue.Get<float>();

	if (AxisValue > 0.5f)
		HandleTargetChange(1);
	else if (AxisValue < -0.5f)
		HandleTargetChange(-1);
}

void AMortisPlayerCharacter::Input_AbilityInputPressed(FGameplayTag InputTag)
{
	if (!bCanAbilityInput) return;

	if (bCanBufferInput && IsBufferableAbility(InputTag))
	{
		BufferedInputTag = InputTag;
		return;
	}

	MortisAbilitySystemComponent->OnAbilityInputPressed(InputTag);
}

void AMortisPlayerCharacter::Input_AbilityInputReleased(FGameplayTag InputTag)
{
	MortisAbilitySystemComponent->OnAbilityInputReleased(InputTag);
}

bool AMortisPlayerCharacter::IsBufferableAbility(FGameplayTag AbilityTag)
{
	return 
		AbilityTag.MatchesTagExact(MortisGameplayTags::InputTag_Ability_Roll)
		|| AbilityTag.MatchesTagExact(MortisGameplayTags::InputTag_Ability_LightAttack)
		|| AbilityTag.MatchesTag(MortisGameplayTags::InputTag_Ability_WeaponSkill);
}

void AMortisPlayerCharacter::ChangeMovementMaxSpeed(const FOnAttributeChangeData& Data)
{
	if (UMortisPlayerMovementComponent* MovementComp = Cast<UMortisPlayerMovementComponent>(GetCharacterMovement()))
		MovementComp->SetTargetSpeed(Data.NewValue);
}

void AMortisPlayerCharacter::HandleTargetChange(int32 Direction)
{
	if (MortisAbilitySystemComponent && MortisAbilitySystemComponent->HasMatchingGameplayTag(MortisGameplayTags::State_LockOn))
	{
		// 락온 오른쪽 타겟 전환
		const FGameplayTag EventTag = (Direction > 0)
			? MortisGameplayTags::Event_Select_Right
			: MortisGameplayTags::Event_Select_Left;
		FGameplayEventData Payload;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, Payload);
		return;
	}

	if (!InteractionComponent)
	{
		return;
	}

	if (Direction > 0)
	{
		InteractionComponent->SelectNextTarget();
	}
	else
	{
		InteractionComponent->SelectPreviousTarget();
	}
}
