// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MortisWeaponBase.h"

#include "MortisDebugHelper.h"
#include "MortisFunctionLibrary.h"

// Sets default values
AMortisWeaponBase::AMortisWeaponBase()
{
}

void AMortisWeaponBase::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();
	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instigator as the owning pawn for the weapon: %s"), *GetName());
	
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UMortisFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
		{
			if (AddUniqueOverlappedActor(OtherActor))
			{
				OnWeaponHitTarget.ExecuteIfBound(OtherActor);
			}
		}
	}
}

void AMortisWeaponBase::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappedActors.Contains(OtherActor))
	{
		OverlappedActors.Remove(OtherActor);
		OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
	}
}