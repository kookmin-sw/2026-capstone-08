// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisWeaponBase.h"
#include "Components/Collisions/MortisBoxComponent.h"
#include "MortisGameplayTags.h"
#include "MortisFunctionLibrary.h"

#include "MortisDebugHelper.h"
#include "Components/Collisions/MortisSphereComponent.h"

// Sets default values
AMortisWeaponBase::AMortisWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	SetRootComponent(WeaponRoot);
}

UShapeComponent* AMortisWeaponBase::GetWeaponCollisionComponent(FGameplayTag TagToToggle)
{
	if (const TObjectPtr<UShapeComponent>* Found = CollisionComponentMap.Find(TagToToggle))
		return Found->Get();
	return nullptr;
}

void AMortisWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeWeaponCollisions();
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

void AMortisWeaponBase::InitializeWeaponCollisions()
{
	CollisionComponentMap.Empty();
	TArray<UMortisBoxComponent*> BoxComponents;
	GetComponents<UMortisBoxComponent>(BoxComponents);
	for (UMortisBoxComponent* BoxComp : BoxComponents)
	{
		if (BoxComp && BoxComp->GetCollisionTag().IsValid())
		{
			BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CollisionComponentMap.Add(BoxComp->GetCollisionTag(), BoxComp);
			BoxComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);
			BoxComp->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxEndOverlap);
		}
	}

	TArray<UMortisSphereComponent*> SphereComponents;
	GetComponents<UMortisSphereComponent>(SphereComponents);
	for (UMortisSphereComponent* SphereComp : SphereComponents)
	{
		if (SphereComp && SphereComp->GetCollisionTag().IsValid())
		{
			SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CollisionComponentMap.Add(SphereComp->GetCollisionTag(), SphereComp);
			SphereComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);
			SphereComp->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxEndOverlap);
		}
	}
}