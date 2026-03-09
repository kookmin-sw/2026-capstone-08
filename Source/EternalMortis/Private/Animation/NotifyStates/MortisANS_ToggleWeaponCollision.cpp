// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyStates/MortisANS_ToggleWeaponCollision.h"

#include "Components/Combat/MortisCombatComponent.h"
#include "Interfaces/MortisCombatInterface.h"

#include "MortisDebugHelper.h"

void UMortisANS_ToggleWeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ToggleWeaponCollisionForMesh(MeshComp, true);
}

void UMortisANS_ToggleWeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ToggleWeaponCollisionForMesh(MeshComp, false);
}

void UMortisANS_ToggleWeaponCollision::ToggleWeaponCollisionForMesh(const USkeletalMeshComponent* MeshComp, bool bEnable) const
{
	if (!MeshComp)
	{
		return;
	}
	
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}
	
	if (const IMortisCombatInterface* CombatInterface = Cast<IMortisCombatInterface>(Owner))
	{
		if (UMortisCombatComponent* CombatComp = CombatInterface->GetCombatComponent())
		{
			CombatComp->ToggleDamageCollision(bEnable, TagToToggle, ToggleDamageType);
		}
	}
}
