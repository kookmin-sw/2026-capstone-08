// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisShieldBase.h"
#include "Components/Collisions/MortisSphereComponent.h"
#include "Items/Weapons/MortisWeaponBase.h"

void AMortisShieldBase::BeginPlay()
{
	Super::BeginPlay();
	
	for (const auto& TagShapeComponentsPair : CollisionComponentMap)
	{
		for (const TObjectPtr<UShapeComponent>& CollisionComp : TagShapeComponentsPair.Value)
		{
			CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
}

void AMortisShieldBase::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnShieldBeginBlock.ExecuteIfBound(OtherActor); 
}

void AMortisShieldBase::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OnShieldEndBlock.ExecuteIfBound(OtherActor);
}
