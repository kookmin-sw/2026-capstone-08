// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Player/MortisGA_TargetLock.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Character/Player/MortisPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UMortisGA_TargetLock::UMortisGA_TargetLock()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UMortisGA_TargetLock::TryLockOnTarget()
{
	TArray<AActor*> FoundActors;
	GetAvailableActorsToLock(FoundActors);

	AvailableActorsToLock.Empty();
	for (AActor* Actor : FoundActors)
	{
		AvailableActorsToLock.Add(Actor);
	}

	CurrentLockedActor = GetBestTargetFromAvailableActors(FoundActors);

	return IsValid(CurrentLockedActor);
}

void UMortisGA_TargetLock::OnTargetLockTick(float DeltaTime)
{
	AMortisPlayerCharacter* PlayerCharacter = GetMortisPlayerCharacterFromActorInfo();
	APlayerController* PlayerController = GetPlayerControllerFromActorInfo();

	if (!PlayerCharacter || !PlayerController || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}

	if (!CanMaintainLockOnTarget(CurrentLockedActor))
	{
		CancelTargetLockAbility();
		return;
	}

	const float DistanceToTarget = FVector::Distance(
		PlayerCharacter->GetActorLocation(),
		CurrentLockedActor->GetActorLocation()
	);

	if (DistanceToTarget > BreakLockDistance)
	{
		CancelTargetLockAbility();
		return;
	}

	SetTargetLockWidgetPosition();

	const bool bShouldOverrideRotation =
		!HasGameplayTagOnActor(PlayerCharacter, RollingStateTag) &&
		!HasGameplayTagOnActor(PlayerCharacter, BlockingStateTag);

	if (!bShouldOverrideRotation)
		return;

	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
		PlayerCharacter->GetActorLocation(),
		CurrentLockedActor->GetActorLocation()
	);

	LookAtRot.Pitch -= TargetLockCameraPitchOffset;

	const FRotator CurrentControlRot = PlayerController->GetControlRotation();
	const FRotator TargetRot = FMath::RInterpTo(
		CurrentControlRot,
		LookAtRot,
		DeltaTime,
		TargetLockRotationInterpSpeed
	);

	PlayerController->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.f));
	PlayerCharacter->SetActorRotation(FRotator(0.f, TargetRot.Yaw, 0.f));
}

void UMortisGA_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{
	if (!CurrentLockedActor)
		return;

	TArray<AActor*> FoundActors;
	GetAvailableActorsToLock(FoundActors);

	AvailableActorsToLock.Empty();
	for (AActor* Actor : FoundActors)
	{
		AvailableActorsToLock.Add(Actor);
	}

	if (FoundActors.IsEmpty())
		return;

	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	GetAvailableActorsOnScreenSides(FoundActors, ActorsOnLeft, ActorsOnRight);

	AActor* NewTarget = nullptr;

	if (InSwitchDirectionTag.MatchesTag(SwitchLeftDirectionTag))
		NewTarget = GetBestTargetFromAvailableActors(ActorsOnLeft);
	else if (InSwitchDirectionTag.MatchesTag(SwitchRightDirectionTag))
		NewTarget = GetBestTargetFromAvailableActors(ActorsOnRight);

	if (NewTarget)
	{
		CurrentLockedActor = NewTarget;
		SetTargetLockWidgetPosition();
	}
}

void UMortisGA_TargetLock::InitializeTargetLockState()
{
	InitializeLockOnMovement();
	ApplyLockOnMoveSpeedEffect();
	DrawTargetLockWidget();
	SetTargetLockWidgetPosition();
}

void UMortisGA_TargetLock::CleanupTargetLockState()
{
	ResetLockOnMovement();
	RemoveLockOnMoveSpeedEffect();

	AvailableActorsToLock.Empty();
	CurrentLockedActor = nullptr;

	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
		DrawnTargetLockWidget = nullptr;
	}
}

void UMortisGA_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UMortisGA_TargetLock::GetAvailableActorsToLock(TArray<AActor*>& OutActors) const
{
	OutActors.Empty();

	const AMortisPlayerCharacter* PlayerCharacter = GetMortisPlayerCharacterFromActorInfo();
	const APlayerController* PlayerController = GetPlayerControllerFromActorInfo();

	if (!PlayerCharacter || !PlayerController)
		return;

	const FVector Start = PlayerCharacter->GetActorLocation();
	const FVector Forward = PlayerController->GetControlRotation().Vector();
	const FVector End = Start + Forward * BoxTraceDistance;

	TArray<FHitResult> BoxTraceHits;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(const_cast<AMortisPlayerCharacter*>(PlayerCharacter));

	UKismetSystemLibrary::BoxTraceMultiForObjects(
		PlayerCharacter,
		Start,
		End,
		TraceBoxSize * 0.5f,
		PlayerController->GetControlRotation(),
		BoxTraceObjectTypes,
		false,
		ActorsToIgnore,
		bShouldPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		BoxTraceHits,
		true
	);

	for (const FHitResult& Hit : BoxTraceHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsSelectableLockOnTarget(HitActor))
			continue;

		if (!HasLineOfSightToTarget(HitActor))
			continue;

		OutActors.AddUnique(HitActor);
	}
}

AActor* UMortisGA_TargetLock::GetBestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors) const
{
	AActor* BestActor = nullptr;
	float BestScore = -FLT_MAX;

	for (AActor* Candidate : InAvailableActors)
	{
		if (!IsValid(Candidate))
		{
			continue;
		}

		const float Score = GetTargetScore(Candidate);
		if (Score > BestScore)
		{
			BestScore = Score;
			BestActor = Candidate;
		}
	}

	return BestActor;
}

void UMortisGA_TargetLock::GetAvailableActorsOnScreenSides(const TArray<AActor*>& InAvailableActors, TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight) const
{
	OutActorsOnLeft.Empty();
	OutActorsOnRight.Empty();

	const AMortisPlayerCharacter* PlayerCharacter = GetMortisPlayerCharacterFromActorInfo();
	const APlayerController* PC = GetPlayerControllerFromActorInfo();

	if (!PlayerCharacter || !PC)
		return;

	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	const FVector ViewRight = FRotationMatrix(PC->GetControlRotation()).GetUnitAxis(EAxis::Y);

	for (AActor* Candidate : InAvailableActors)
	{
		if (!Candidate || Candidate == CurrentLockedActor)
			continue;

		const FVector ToCandidate = (Candidate->GetActorLocation() - PlayerLocation).GetSafeNormal();
		const float SideDot = FVector::DotProduct(ViewRight, ToCandidate);

		if (SideDot < -KINDA_SMALL_NUMBER)
			OutActorsOnLeft.AddUnique(Candidate);
		else if (SideDot > KINDA_SMALL_NUMBER)
			OutActorsOnRight.AddUnique(Candidate);
	}
}

bool UMortisGA_TargetLock::IsSelectableLockOnTarget(AActor* InActor) const
{
	const AMortisPlayerCharacter* PlayerCharacter = GetMortisPlayerCharacterFromActorInfo();
	const APlayerController* PlayerController = GetPlayerControllerFromActorInfo();

	if (!PlayerCharacter || !PlayerController || !IsValid(InActor) || InActor == PlayerCharacter)
		return false;

	if (DeadStateTag.IsValid() && HasGameplayTagOnActor(InActor, DeadStateTag))
		return false;

	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector ToTarget = (InActor->GetActorLocation() - ViewLocation).GetSafeNormal();
	const float ViewDot = FVector::DotProduct(ViewRotation.Vector(), ToTarget);

	return ViewDot >= MinTargetViewDot;
}

bool UMortisGA_TargetLock::CanMaintainLockOnTarget(AActor* InActor) const
{
	if (!IsValid(InActor))
		return false;

	if (DeadStateTag.IsValid() && HasGameplayTagOnActor(InActor, DeadStateTag))
		return false;

	return HasLineOfSightToTarget(InActor);
}

bool UMortisGA_TargetLock::HasGameplayTagOnActor(const AActor* InActor, const FGameplayTag& InTag) const
{
	if (!InActor || !InTag.IsValid())
		return false;

	const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(InActor);
	if (!ASI)
		return false;

	const UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	return ASC && ASC->HasMatchingGameplayTag(InTag);
}

bool UMortisGA_TargetLock::HasLineOfSightToTarget(AActor* InActor) const
{
	const APlayerController* PlayerController = GetPlayerControllerFromActorInfo();
	const AMortisPlayerCharacter* PlayerCharacter = GetMortisPlayerCharacterFromActorInfo();

	if (!PlayerController || !PlayerCharacter || !InActor)
		return false;

	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector TargetLocation = InActor->GetActorLocation() + FVector(0.f, 0.f, 60.f);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(TargetLockLOS), false, PlayerCharacter);
	Params.AddIgnoredActor(PlayerCharacter);

	const bool bBlockingHit = PlayerCharacter->GetWorld()->LineTraceSingleByChannel(
		Hit,
		ViewLocation,
		TargetLocation,
		LineOfSightTraceChannel,
		Params
	);

	if (!bBlockingHit)
		return true;

	return Hit.GetActor() == InActor;
}

float UMortisGA_TargetLock::GetTargetScore(AActor* InActor) const
{
	const APlayerController* PC = GetPlayerControllerFromActorInfo();
	if (!PC || !InActor)
	{
		return -FLT_MAX;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	PC->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector ToTarget = InActor->GetActorLocation() - ViewLocation;
	const float Distance = ToTarget.Length();
	const float Dot = FVector::DotProduct(ViewRotation.Vector(), ToTarget.GetSafeNormal());

	return (Dot * 10000.f) - (Distance * 0.5f);
}

void UMortisGA_TargetLock::DrawTargetLockWidget()
{
	if (DrawnTargetLockWidget || !TargetLockWidgetClass)
	{
		return;
	}

	if (APlayerController* PC = GetPlayerControllerFromActorInfo())
	{
		DrawnTargetLockWidget = CreateWidget<UUserWidget>(PC, TargetLockWidgetClass);
		if (DrawnTargetLockWidget)
		{
			DrawnTargetLockWidget->AddToViewport();
			DrawnTargetLockWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
		}
	}
}

void UMortisGA_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawnTargetLockWidget || !CurrentLockedActor)
	{
		return;
	}

	APlayerController* PC = GetPlayerControllerFromActorInfo();
	if (!PC)
	{
		return;
	}

	FVector2D ScreenPosition;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		PC,
		CurrentLockedActor->GetActorLocation() + TargetLockWidgetWorldOffset,
		ScreenPosition,
		true
	);

	DrawnTargetLockWidget->SetPositionInViewport(ScreenPosition, false);
}

void UMortisGA_TargetLock::InitializeLockOnMovement()
{
	AMortisPlayerCharacter* PlayerCharacter = GetMortisPlayerCharacterFromActorInfo();
	if (!PlayerCharacter)
	{
		return;
	}

	bCachedUseControllerRotationYaw = PlayerCharacter->bUseControllerRotationYaw;
	PlayerCharacter->bUseControllerRotationYaw = false;

	if (UCharacterMovementComponent* MovementComp = PlayerCharacter->GetCharacterMovement())
	{
		bCachedOrientRotationToMovement = MovementComp->bOrientRotationToMovement;
		MovementComp->bOrientRotationToMovement = false;
	}
}

void UMortisGA_TargetLock::ResetLockOnMovement()
{
	AMortisPlayerCharacter* PlayerCharacter = GetMortisPlayerCharacterFromActorInfo();
	if (!PlayerCharacter)
	{
		return;
	}

	PlayerCharacter->bUseControllerRotationYaw = bCachedUseControllerRotationYaw;

	if (UCharacterMovementComponent* MovementComp = PlayerCharacter->GetCharacterMovement())
	{
		MovementComp->bOrientRotationToMovement = bCachedOrientRotationToMovement;
	}
}

void UMortisGA_TargetLock::ApplyLockOnMoveSpeedEffect()
{
	if (!LockOnMoveSpeedEffectClass)
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	LockOnMoveSpeedEffectHandle = BP_ApplyGameplayEffectToOwner(
		LockOnMoveSpeedEffectClass,
		1.f,
		1
	);
}

void UMortisGA_TargetLock::RemoveLockOnMoveSpeedEffect()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !LockOnMoveSpeedEffectHandle.IsValid())
	{
		return;
	}

	ASC->RemoveActiveGameplayEffect(LockOnMoveSpeedEffectHandle);
	LockOnMoveSpeedEffectHandle.Invalidate();
}

AMortisPlayerCharacter* UMortisGA_TargetLock::GetMortisPlayerCharacterFromActorInfo() const
{
	return Cast<AMortisPlayerCharacter>(GetAvatarActorFromActorInfo());
}

APlayerController* UMortisGA_TargetLock::GetPlayerControllerFromActorInfo() const
{
	return CurrentActorInfo ? Cast<APlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr;
}








