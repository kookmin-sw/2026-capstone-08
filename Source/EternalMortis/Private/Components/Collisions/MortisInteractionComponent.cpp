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

	if (!IsValid(SelectedTarget) || !InteractionCandidates.Contains(SelectedTarget))
		ReselectIfNeeded();
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
	if (InteractionCandidates.Num() > 0)
		SetSelectedTarget(InteractionCandidates[0]);
	else
		SetSelectedTarget(nullptr);
}

bool UMortisInteractionComponent::IsCandidateAllowed(AMortisInteractableActorBase* Target, APawn* OwnerPawn) const
{
	if (!OwnerPawn || !IsValid(Target))
	{
		return false;
	}

	return Target->CanBeInteractionCandidate(OwnerPawn) && Target->CanInteract(OwnerPawn);
}
