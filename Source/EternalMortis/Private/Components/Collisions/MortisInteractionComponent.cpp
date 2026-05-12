// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Collisions/MortisInteractionComponent.h"
#include "Items/Interactable/MortisInteractableActorBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include "MortisDebugHelper.h"

void UMortisInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::HandleBeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::HandleEndOverlap);

	RefreshCandidatesFromOverlaps();
}

void UMortisInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::HandleBeginOverlap);
	OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::HandleEndOverlap);

	Super::EndPlay(EndPlayReason);
}

void UMortisInteractionComponent::LockSelectedAsPending()
{
	RefreshCandidatesFromOverlaps();
	PendingTarget = SelectedTarget;
}

void UMortisInteractionComponent::ClearPendingTarget()
{
	PendingTarget = nullptr;
}

void UMortisInteractionComponent::SelectNextTarget()
{
	RefreshCandidatesFromOverlaps();

	if (InteractionCandidates.Num() == 0)
	{
		SetSelectedTarget(nullptr);
		return;
	}

	if (!SelectedTarget)
	{
		SetSelectedTarget(InteractionCandidates[0]);
		return;
	}

	const int32 CurrentIndex = InteractionCandidates.IndexOfByKey(SelectedTarget);
	if (CurrentIndex == INDEX_NONE)
	{
		SetSelectedTarget(InteractionCandidates[0]);
		return;
	}

	const int32 NewIndex = (CurrentIndex + 1) % InteractionCandidates.Num();
	SetSelectedTarget(InteractionCandidates[NewIndex]);
}

void UMortisInteractionComponent::SelectPreviousTarget()
{
	RefreshCandidatesFromOverlaps();

	if (InteractionCandidates.Num() == 0)
	{
		SetSelectedTarget(nullptr);
		return;
	}

	if (!SelectedTarget)
	{
		SetSelectedTarget(InteractionCandidates[0]);
		return;
	}

	const int32 CurrentIndex = InteractionCandidates.IndexOfByKey(SelectedTarget);
	if (CurrentIndex == INDEX_NONE)
	{
		SetSelectedTarget(InteractionCandidates[0]);
		return;
	}

	const int32 NewIndex = (CurrentIndex - 1 + InteractionCandidates.Num()) % InteractionCandidates.Num();
	SetSelectedTarget(InteractionCandidates[NewIndex]);
}

void UMortisInteractionComponent::ClearAllCandidates()
{
	InteractionCandidates.Empty();
	SetSelectedTarget(nullptr);
}

void UMortisInteractionComponent::RefreshCandidatesFromOverlaps()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		ClearAllCandidates();
		PendingTarget = nullptr;
		return;
	}

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AMortisInteractableActorBase::StaticClass());

	TArray<TObjectPtr<AMortisInteractableActorBase>> NewCandidates;
	NewCandidates.Reserve(OverlappingActors.Num());

	for (AActor* OverlappedActor : OverlappingActors)
	{
		AMortisInteractableActorBase* Target = Cast<AMortisInteractableActorBase>(OverlappedActor);
		if (!Target)
			continue;

		if (!IsCandidateAllowed(Target, OwnerPawn))
			continue;

		NewCandidates.AddUnique(Target);
	}

	InteractionCandidates = MoveTemp(NewCandidates);

	SortCandidatesByViewAngle(OwnerPawn);

	if (!IsValid(SelectedTarget) || !InteractionCandidates.Contains(SelectedTarget))
		SetSelectedTarget(GetBestCandidate(OwnerPawn));
}

void UMortisInteractionComponent::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner())
		return;

	AMortisInteractableActorBase* NewTarget = Cast<AMortisInteractableActorBase>(OtherActor);
	if (!NewTarget)
		return;

	RefreshCandidatesFromOverlaps();
}

void UMortisInteractionComponent::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == GetOwner())
		return;

	AMortisInteractableActorBase* RemoveTarget = Cast<AMortisInteractableActorBase>(OtherActor);
	if (!RemoveTarget)
		return;

	RefreshCandidatesFromOverlaps();
}

void UMortisInteractionComponent::SetSelectedTarget(AMortisInteractableActorBase* NewTarget)
{
	if (SelectedTarget == NewTarget)
		return;

	if (IsValid(SelectedTarget))
	{
		SelectedTarget->SetSelectionIndicatorVisible(false);
	}

	SelectedTarget = NewTarget;

	if (IsValid(SelectedTarget))
	{
		SelectedTarget->SetSelectionIndicatorVisible(true);
	}
}

void UMortisInteractionComponent::ReselectIfNeeded()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || InteractionCandidates.Num() == 0)
	{
		SetSelectedTarget(nullptr);
		return;
	}

	SetSelectedTarget(GetBestCandidate(OwnerPawn));
}

bool UMortisInteractionComponent::IsCandidateAllowed(AMortisInteractableActorBase* Target, APawn* OwnerPawn) const
{
	if (!OwnerPawn || !IsValid(Target))
	{
		return false;
	}

	return Target->CanBeInteractionCandidate(OwnerPawn) && Target->CanInteract(OwnerPawn);
}

float UMortisInteractionComponent::GetCandidateViewAngle(const AMortisInteractableActorBase* Target, const APawn* OwnerPawn) const
{
	if (!Target || !OwnerPawn)
		return 0.f;

	FVector Forward = OwnerPawn->GetActorForwardVector();
	FVector Right = OwnerPawn->GetActorRightVector();

	if (const AController* Controller = OwnerPawn->GetController())
	{
		const FRotator ControlRot = Controller->GetControlRotation();
		const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

		Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	}

	const FVector ToTarget = (Target->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal();

	const float ForwardDot = FVector::DotProduct(Forward, ToTarget);
	const float RightDot = FVector::DotProduct(Right, ToTarget);

	// -180 ~ 180
	// 음수: 왼쪽, 0: 정면, 양수: 오른쪽
	return FMath::RadiansToDegrees(FMath::Atan2(RightDot, ForwardDot));
}

float UMortisInteractionComponent::GetCandidateScore(AMortisInteractableActorBase* Target, const APawn* OwnerPawn) const
{
	if (!Target || !OwnerPawn)
		return -FLT_MAX;

	FVector Forward = OwnerPawn->GetActorForwardVector();

	if (const AController* Controller = OwnerPawn->GetController())
	{
		const FRotator ControlRot = Controller->GetControlRotation();
		const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
		Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	}

	const FVector ToTarget = Target->GetActorLocation() - OwnerPawn->GetActorLocation();
	const float Distance = ToTarget.Length();
	const float Dot = FVector::DotProduct(Forward, ToTarget.GetSafeNormal());

	// 정면일수록, 가까울수록 우선
	return (Dot * 10000.f) - (Distance * 0.5f);
}

void UMortisInteractionComponent::SortCandidatesByViewAngle(APawn* OwnerPawn)
{
	if (!OwnerPawn)
		return;

	InteractionCandidates.Sort(
		[this, OwnerPawn](const AMortisInteractableActorBase& A, const AMortisInteractableActorBase& B)
		{
			return GetCandidateViewAngle(&A, OwnerPawn) < GetCandidateViewAngle(&B, OwnerPawn);
		}
	);
}

AMortisInteractableActorBase* UMortisInteractionComponent::GetBestCandidate(APawn* OwnerPawn) const
{
	if (!OwnerPawn)
		return nullptr;

	AMortisInteractableActorBase* BestTarget = nullptr;
	float BestScore = -FLT_MAX;

	for (AMortisInteractableActorBase* Candidate : InteractionCandidates)
	{
		if (!IsValid(Candidate))
			continue;

		const float Score = GetCandidateScore(Candidate, OwnerPawn);
		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = Candidate;
		}
	}

	return BestTarget;
}
