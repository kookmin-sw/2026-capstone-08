// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisCombatItemBase.h"

#include "MortisDebugHelper.h"
#include "Components/ShapeComponent.h"
#include "Interfaces/MortisCollisionInterface.h"

// AMortisCombatItemBase::AMortisCombatItemBase()
// {
// 	PrimaryActorTick.bCanEverTick = false;
//
// 	ItemRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ItemRoot"));
// 	SetRootComponent(ItemRoot);
// }
//
// void AMortisCombatItemBase::ClearOverlappedActors()
// {
// 	OverlappedActors.Empty();
// }
//
// const TArray<TObjectPtr<UShapeComponent>>* AMortisCombatItemBase::GetCollisionComponentsByTag(FGameplayTag TagToToggle)
// {
// 	if (const TArray<TObjectPtr<UShapeComponent>>* Found = CollisionComponentMap.Find(TagToToggle))
// 	{
// 		return Found;
// 	}
// 	return nullptr;
// }
//
// bool AMortisCombatItemBase::AddUniqueOverlappedActor(AActor* NewActor)
// {
// 	if (NewActor && !OverlappedActors.Contains(NewActor))
// 	{
// 		OverlappedActors.Add(NewActor);
// 		return true;
// 	}
// 	return false;
// }
//
// void AMortisCombatItemBase::BeginPlay()
// {
// 	Super::BeginPlay();
// 	
// 	InitializeItemMesh();
// 	InitializeCollisions();
// }
//
// void AMortisCombatItemBase::InitializeCollisions()
// {
// 	CollisionComponentMap.Empty();
// 	TArray<UShapeComponent*> ShapeComponents;
// 	GetComponents<UShapeComponent>(ShapeComponents);
// 	for (UShapeComponent* ShapeComp : ShapeComponents)
// 	{
// 		if (IMortisCollisionInterface* CollisionComp = Cast<IMortisCollisionInterface>(ShapeComp))
// 		{
// 			if (!CollisionComp->GetCollisionTag().IsValid())
// 			{
// 				continue;
// 			}
// 			
// 			CollisionComponentMap.FindOrAdd(CollisionComp->GetCollisionTag()).Add(ShapeComp);
// 			ShapeComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBeginOverlap);
// 			ShapeComp->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionEndOverlap);
// 			ShapeComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
// 		}
// 	}
// }
//
// void AMortisCombatItemBase::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// }
//
// void AMortisCombatItemBase::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
// {
// }
//
// void AMortisCombatItemBase::InitializeItemMesh()
// {
// 	if (UStaticMeshComponent* StaticMeshComp = GetComponentByClass<UStaticMeshComponent>())
// 	{
// 		ItemMesh = StaticMeshComp;
// 	}
// 	else if (USkeletalMeshComponent* SkeletalMeshComp = GetComponentByClass<USkeletalMeshComponent>())
// 	{
// 		ItemMesh = SkeletalMeshComp;
// 	}
// 	else
// 	{
// 		MORTIS_LOG("Item Mesh is not set!");
// 		check(false);
// 	}
// 	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
// }
