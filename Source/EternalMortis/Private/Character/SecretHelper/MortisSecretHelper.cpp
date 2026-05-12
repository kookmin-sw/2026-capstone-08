// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SecretHelper/MortisSecretHelper.h"
#include "Components/SceneComponent.h"
#include "Items/Interactable/Pickup/MortisPickupBase.h"
#include "Items/Interactable/Pickup/SecretHelper/MortisSecretHelperRunePickup.h"
#include "Items/Interactable/Pickup/SecretHelper/MortisSecretHelperWeaponPickup.h"

AMortisSecretHelper::AMortisSecretHelper()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	PickupSpawnPoint_0 = CreateDefaultSubobject<USceneComponent>(TEXT("PickupSpawnPoint_0"));
	PickupSpawnPoint_0->SetupAttachment(SceneRoot);
	PickupSpawnPoint_0->SetRelativeLocation(FVector(0.f, -120.f, 0.f));

	PickupSpawnPoint_1 = CreateDefaultSubobject<USceneComponent>(TEXT("PickupSpawnPoint_1"));
	PickupSpawnPoint_1->SetupAttachment(SceneRoot);
	PickupSpawnPoint_1->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

	PickupSpawnPoint_2 = CreateDefaultSubobject<USceneComponent>(TEXT("PickupSpawnPoint_2"));
	PickupSpawnPoint_2->SetupAttachment(SceneRoot);
	PickupSpawnPoint_2->SetRelativeLocation(FVector(0.f, 120.f, 0.f));
}

void AMortisSecretHelper::RegisterOwnedPickup(AMortisPickupBase* Pickup)
{
	if (!IsValid(Pickup))
		return;

	OwnedPickups.AddUnique(Pickup);

	if (AMortisSecretHelperRunePickup* RunePickup = Cast<AMortisSecretHelperRunePickup>(Pickup))
		RunePickup->SetOwningSecretHelper(this);
	else if (AMortisSecretHelperWeaponPickup* WeaponPickup = Cast<AMortisSecretHelperWeaponPickup>(Pickup))
		WeaponPickup->SetOwningSecretHelper(this);
}

void AMortisSecretHelper::UnregisterOwnedPickup(AMortisPickupBase* Pickup)
{
	if (!IsValid(Pickup))
		return;

	if (AMortisSecretHelperRunePickup* RunePickup = Cast<AMortisSecretHelperRunePickup>(Pickup))
		RunePickup->SetOwningSecretHelper(nullptr);
	else if (AMortisSecretHelperWeaponPickup* WeaponPickup = Cast<AMortisSecretHelperWeaponPickup>(Pickup))
		WeaponPickup->SetOwningSecretHelper(nullptr);

	OwnedPickups.Remove(Pickup);
}

void AMortisSecretHelper::NotifyOwnedPickupInteracted(AMortisPickupBase* InteractedPickup, APawn* InteractingPawn)
{
	if (bSelectionResolved)
		return;

	bSelectionResolved = true;

	for (AMortisPickupBase* Pickup : OwnedPickups)
	{
		if (!IsValid(Pickup))
			continue;

		Pickup->SetSelectionIndicatorVisible(false);
		Pickup->Destroy();
	}

	OwnedPickups.Reset();

	if (bDestroySelfAfterSelection)
		Destroy();
}

void AMortisSecretHelper::BeginPlay()
{
	Super::BeginPlay();
}

void AMortisSecretHelper::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OwnedPickups.Reset();

	Super::EndPlay(EndPlayReason);
}
