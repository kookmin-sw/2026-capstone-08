// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Collisions/MortisInteractionComponent.h"
#include "Items/Interactable/MortisInteractableActorBase.h"
#include "GameFramework/Pawn.h"

#include "MortisDebugHelper.h"

void UMortisInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::HandleBeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::HandleEndOverlap);

	CleanupInvalidCandidates();
}

void UMortisInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::HandleBeginOverlap);
	OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::HandleEndOverlap);

	Super::EndPlay(EndPlayReason);
}

void UMortisInteractionComponent::LockSelectedAsPending()
{
	CleanupInvalidCandidates();
	PendingTarget = SelectedTarget;
}

void UMortisInteractionComponent::ClearPendingTarget()
{
	PendingTarget = nullptr;
}

void UMortisInteractionComponent::SelectNextTarget()
{
	CleanupInvalidCandidates();

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
	CleanupInvalidCandidates();

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

void UMortisInteractionComponent::CleanupInvalidCandidates()
{
	for (int32 i = InteractionCandidates.Num() - 1; i >= 0; --i)
	{
		AMortisInteractableActorBase* Candidate = InteractionCandidates[i];
		if (!IsTargetStillUsable(Candidate))
			InteractionCandidates.RemoveAt(i);
	}

	if (!IsValid(SelectedTarget) || !InteractionCandidates.Contains(SelectedTarget))
		ReselectIfNeeded();

	if (PendingTarget && !IsValid(PendingTarget))
		PendingTarget = nullptr;
}

void UMortisInteractionComponent::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner())
		return;

	AMortisInteractableActorBase* NewTarget = Cast<AMortisInteractableActorBase>(OtherActor);
	if (!NewTarget)
		return;

	AddCandidate(NewTarget);
	MORTIS_LOG("Item Added");
}

void UMortisInteractionComponent::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == GetOwner())
		return;

	AMortisInteractableActorBase* RemoveTarget = Cast<AMortisInteractableActorBase>(OtherActor);
	if (!RemoveTarget)
		return;

	RemoveCandidate(RemoveTarget);
	MORTIS_LOG("Item Removed");
}

void UMortisInteractionComponent::SetSelectedTarget(AMortisInteractableActorBase* NewTarget)
{
	if (SelectedTarget == NewTarget)
		return;

	if (IsValid(SelectedTarget))
	{
		SelectedTarget->SetSelectionUIVisible(false);
	}

	SelectedTarget = NewTarget;

	if (IsValid(SelectedTarget))
	{
		SelectedTarget->SetSelectionUIVisible(true);
	}
}

void UMortisInteractionComponent::AddCandidate(AMortisInteractableActorBase* NewTarget)
{
	if (!IsTargetStillUsable(NewTarget))
		return;

	if (InteractionCandidates.Contains(NewTarget))
		return;

	InteractionCandidates.Add(NewTarget);

	if (!SelectedTarget)
		SetSelectedTarget(NewTarget);
}

void UMortisInteractionComponent::RemoveCandidate(AMortisInteractableActorBase* RemoveTarget)
{
	if (!RemoveTarget)
		return;

	const bool bWasSelected = (SelectedTarget == RemoveTarget);

	InteractionCandidates.Remove(RemoveTarget);

	if (bWasSelected)
		ReselectIfNeeded();
}

void UMortisInteractionComponent::ReselectIfNeeded()
{
	if (InteractionCandidates.Num() > 0)
		SetSelectedTarget(InteractionCandidates[0]);
	else
		SetSelectedTarget(nullptr);
}

bool UMortisInteractionComponent::IsTargetStillUsable(AMortisInteractableActorBase* Target) const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	if (!OwnerPawn || !IsValid(Target))
		return false;

	return Target->CanInteract(OwnerPawn);
}
