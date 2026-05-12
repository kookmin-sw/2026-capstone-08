// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyStates/MortisANS_SocketTrail.h"

#include "MortisFunctionLibrary.h"
#include "Character/MortisCharacterBase.h"
#include "Components/Combat/MortisCombatComponent.h"
#include "Particles/ParticleSystemComponent.h"

void UMortisANS_SocketTrail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	switch (MeshSource)
	{
	case EMortisMeshSource::WeaponMesh:
		if (AMortisWeaponBase* Weapon = UMortisFunctionLibrary::GetWeaponFromCurrentSlot(MeshComp))
		{
			Weapon->StartWeaponCascadeTrail(TrailStartSocketName, TrailEndSocketName, TrailWidth);
		}
		break;
	case EMortisMeshSource::CharacterMesh:
		ToggleTrailFromCharacterMesh(MeshComp, true);
		break;
	}

}

void UMortisANS_SocketTrail::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	switch (MeshSource)
	{
	case EMortisMeshSource::WeaponMesh:
		if (AMortisWeaponBase* Weapon = UMortisFunctionLibrary::GetWeaponFromCurrentSlot(MeshComp))
		{
			Weapon->EndWeaponCascadeTrail();
		}
		break;
	case EMortisMeshSource::CharacterMesh:
		ToggleTrailFromCharacterMesh(MeshComp, false);
		break;
	}
}

void UMortisANS_SocketTrail::ToggleTrailFromCharacterMesh(USkeletalMeshComponent* MeshComp, bool bEnable) const
{
	if (!MeshComp)
	{
		return;
	}
	
	AMortisCharacterBase* Character = Cast<AMortisCharacterBase>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}
	
	if (Character->GetTrailCascadeComponent())
	{
		if (bEnable)
		{
			Character->GetTrailCascadeComponent()->BeginTrails(TrailStartSocketName, TrailEndSocketName, ETrailWidthMode_FromCentre, TrailWidth);
			// MORTIS_LOG("Start Trail");
		}
		else
		{
			Character->GetTrailCascadeComponent()->EndTrails();
			// MORTIS_LOG("End Trail");
		}
	}
}
