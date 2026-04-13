// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisWeaponBase.h"

#include "MortisDebugHelper.h"
#include "MortisFunctionLibrary.h"
#include "Components/ShapeComponent.h"
#include "Interfaces/MortisCollisionInterface.h"

// Sets default values
AMortisWeaponBase::AMortisWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	SetRootComponent(WeaponRoot);
}

const TArray<TObjectPtr<UShapeComponent>>* AMortisWeaponBase::GetCollisionComponentsByTag(FGameplayTag TagToToggle)
{
	if (const TArray<TObjectPtr<UShapeComponent>>* Found = CollisionComponentMap.Find(TagToToggle))
	{
		return Found;
	}
	return nullptr;
}

void AMortisWeaponBase::ToggleCollision(bool bEnable, const FGameplayTag& TagToToggle)
{
	const TArray<TObjectPtr<UShapeComponent>>* CollisionShapes = CollisionComponentMap.Find(TagToToggle);
	if (CollisionShapes)
	{
		for (const TObjectPtr<UShapeComponent>& CollisionShape : *CollisionShapes)
		{
			CollisionShape->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		}
	}
}

void AMortisWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeWeaponMesh();
	InitializeCollisions();
}

void AMortisWeaponBase::InitializeCollisions()
{
	CollisionComponentMap.Empty();
	TArray<UShapeComponent*> ShapeComponents;
	GetComponents<UShapeComponent>(ShapeComponents);
	for (UShapeComponent* ShapeComp : ShapeComponents)
	{
		if (IMortisCollisionInterface* CollisionComp = Cast<IMortisCollisionInterface>(ShapeComp))
		{
			if (!CollisionComp->GetCollisionTag().IsValid())
			{
				continue;
			}
			
			CollisionComponentMap.FindOrAdd(CollisionComp->GetCollisionTag()).Add(ShapeComp);
			ShapeComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBeginOverlap);
			ShapeComp->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionEndOverlap);
			ShapeComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AMortisWeaponBase::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();
	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instigator as the owning pawn for the weapon: %s"), *GetName());
	
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UMortisFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
		{
			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
		}
	}
}

void AMortisWeaponBase::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
}

void AMortisWeaponBase::InitializeWeaponMesh()
{
	if (UStaticMeshComponent* StaticMeshComp = GetComponentByClass<UStaticMeshComponent>())
	{
		WeaponMesh = StaticMeshComp;
	}
	else if (USkeletalMeshComponent* SkeletalMeshComp = GetComponentByClass<USkeletalMeshComponent>())
	{
		WeaponMesh = SkeletalMeshComp;
	}
	else
	{
		MORTIS_LOG("%s Mesh is not Set", *GetActorNameOrLabel());
		return;
	}
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
}