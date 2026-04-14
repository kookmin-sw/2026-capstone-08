// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/MortisInteractableActorBase.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AMortisInteractableActorBase::AMortisInteractableActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(Root);

	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(Root);
	InteractionCollision->SetBoxExtent(FVector(140.f, 140.f, 100.f));
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);			// 모두 무시시키고
	InteractionCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);		// Pawn과 Overlap만 감지
	InteractionCollision->SetGenerateOverlapEvents(true);
}

bool AMortisInteractableActorBase::CanInteract(APawn* InteractingPawn) const
{
	return bCanInteract;
}

TSubclassOf<UGameplayAbility> AMortisInteractableActorBase::GetInteractionAbility() const
{
	return InteractionAbilityClass;
}

void AMortisInteractableActorBase::OnInteractionReserved(APawn* InteractingPawn)
{
	if (bDisableOnReserve)
		DisableInteraction();
}

void AMortisInteractableActorBase::OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded)
{
	if (!bSucceeded)
		EnableInteraction();
}

void AMortisInteractableActorBase::EnableInteraction()
{
	bCanInteract = true;

	if (InteractionCollision)
	{
		InteractionCollision->SetGenerateOverlapEvents(true);
	}
}

void AMortisInteractableActorBase::DisableInteraction()
{
	bCanInteract = false;

	SetSelectionIndicatorVisible(false);

	if (InteractionCollision)
	{
		InteractionCollision->SetGenerateOverlapEvents(false);
	}
}

bool AMortisInteractableActorBase::IsInteractionEnabled() const
{
	return bCanInteract;
}

void AMortisInteractableActorBase::SetSelectionIndicatorVisible(bool bVisible)
{
	// 기본적으로 아무것도 안하니 Override하여 사용할 것
}

bool AMortisInteractableActorBase::CanBeInteractionCandidate(APawn* InteractingPawn) const
{
	// 일반 아이템류는 기존처럼 그냥 후보 가능
	return true;
}
