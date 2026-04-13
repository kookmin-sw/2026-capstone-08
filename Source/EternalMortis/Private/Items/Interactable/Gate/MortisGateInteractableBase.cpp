// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Interactable/Gate/MortisGateInteractableBase.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Collisions/MortisInteractionComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"

AMortisGateInteractableBase::AMortisGateInteractableBase()
{
	// 부모의 InteractionCollision 재사용
	if (InteractionCollision)
	{
		InteractionCollision->SetBoxExtent(InteractionExtent);
		InteractionCollision->SetRelativeLocation(InteractionOffset);
		InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
		InteractionCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		InteractionCollision->SetGenerateOverlapEvents(true);
	}

	GateBlockCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("GateBlockCollision"));
	GateBlockCollision->SetupAttachment(Root);
	GateBlockCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GateBlockCollision->SetCollisionResponseToAllChannels(ECR_Block);
	GateBlockCollision->SetGenerateOverlapEvents(false);

	PassEndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PassEndPoint"));
	PassEndPoint->SetupAttachment(Root);
}

void AMortisGateInteractableBase::BeginPlay()
{
	Super::BeginPlay();

	if (InteractionCollision)
	{
		InteractionCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::HandleGateZoneBeginOverlap);
		InteractionCollision->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::HandleGateZoneEndOverlap);
	}
}

void AMortisGateInteractableBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (InteractionCollision)
	{
		InteractionCollision->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::HandleGateZoneBeginOverlap);
		InteractionCollision->OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::HandleGateZoneEndOverlap);
	}

	Super::EndPlay(EndPlayReason);
}

bool AMortisGateInteractableBase::CanInteract(APawn* InteractingPawn) const
{
	if (!Super::CanInteract(InteractingPawn) || !InteractingPawn)
	{
		return false;
	}

	if (!CanBeInteractionCandidate(InteractingPawn))
	{
		return false;
	}

	return true;
}

bool AMortisGateInteractableBase::CanBeInteractionCandidate(APawn* InteractingPawn) const
{
	return IsPawnInsideGateInteractionZone(InteractingPawn);;
}

void AMortisGateInteractableBase::HandleGateZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	if (OtherComp != Character->GetCapsuleComponent())
	{
		return;
	}

	if (UMortisInteractionComponent* InteractionComp = Character->FindComponentByClass<UMortisInteractionComponent>())
	{
		InteractionComp->RefreshCandidatesFromOverlaps();
	}
}

void AMortisGateInteractableBase::HandleGateZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	if (OtherComp != Character->GetCapsuleComponent())
	{
		return;
	}

	if (UMortisInteractionComponent* InteractionComp = Character->FindComponentByClass<UMortisInteractionComponent>())
	{
		InteractionComp->RefreshCandidatesFromOverlaps();
	}
}

bool AMortisGateInteractableBase::IsPawnInsideGateInteractionZone(APawn* InteractingPawn) const
{
	const ACharacter* Character = Cast<ACharacter>(InteractingPawn);
	if (!InteractionCollision || !Character)
	{
		return false;
	}

	const UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent();
	if (!CapsuleComp)
	{
		return false;
	}

	return InteractionCollision->IsOverlappingComponent(CapsuleComp);
}
