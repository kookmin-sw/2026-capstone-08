// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisShieldBase.h"
#include "Components/Collisions/MortisSphereComponent.h"
#include "Items/Weapons/MortisWeaponBase.h"

class AMortisWeaponBase;

void AMortisShieldBase::BeginPlay()
{
	Super::BeginPlay();
	
	for (auto& TagCollisionPair : CollisionComponentMap)
	{
		TagCollisionPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AMortisShieldBase::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IMortisCollisionInterface* CollisionComp = Cast<IMortisCollisionInterface>(OtherComp))
	{
		if (AMortisWeaponBase* Weapon = Cast<AMortisWeaponBase>(OtherComp->GetOwner()))
		{
			if (AddUniqueOverlappedActor(Weapon))
			{
				OnShieldBeginBlock.ExecuteIfBound(OtherActor); 
			}
		}
	}
}

void AMortisShieldBase::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappedActors.Contains(OtherActor))
	{
		OverlappedActors.Remove(OtherActor);
		OnShieldEndBlock.ExecuteIfBound(OtherActor);
	}
}
