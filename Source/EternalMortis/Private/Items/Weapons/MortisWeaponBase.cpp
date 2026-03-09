// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisWeaponBase.h"
#include "Components/Collisions/MortisBoxComponent.h"
#include "MortisGameplayTags.h"
#include "MortisFunctionLibrary.h"

#include "MortisDebugHelper.h"

// Sets default values
AMortisWeaponBase::AMortisWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponCollisionBox = CreateDefaultSubobject<UMortisBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxEndOverlap);
	WeaponCollisionBox->CollisionTag = MortisGameplayTags::Data_CollisionType_Weapon_Normal;
}

UBoxComponent* AMortisWeaponBase::GetWeaponCollisionBox(FGameplayTag TagToToggle)
{
	if (const TObjectPtr<UMortisBoxComponent>* Found = CollisionCache.Find(TagToToggle))
		return Found->Get();
	return nullptr;
}

void AMortisWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	CollisionCache.Reset();

	TArray<UMortisBoxComponent*> Hitboxes;
	GetComponents<UMortisBoxComponent>(Hitboxes);

	for (UMortisBoxComponent* Hitbox : Hitboxes)
	{
		if (!Hitbox || !Hitbox->CollisionTag.IsValid())
			continue;

		CollisionCache.Add(Hitbox->CollisionTag, Hitbox);
	}
}

void AMortisWeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();
	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instigator as the owning pawn for the weapon: %s"), *GetName());


	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UMortisFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
	}
}

void AMortisWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();
	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instigator as the owning pawn for the weapon: %s"), *GetName());


	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UMortisFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
	}
}
