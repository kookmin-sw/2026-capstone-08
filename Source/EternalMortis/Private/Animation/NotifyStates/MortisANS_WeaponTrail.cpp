// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyStates/MortisANS_WeaponTrail.h"
#include "Character/MortisCharacterBase.h"
#include "Components/Combat/MortisCombatComponent.h"

void UMortisANS_WeaponTrail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (AMortisWeaponBase* Weapon = GetEquippedWeapon(MeshComp))
	{
		Weapon->StartWeaponTrail();
	}
}

void UMortisANS_WeaponTrail::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AMortisWeaponBase* Weapon = GetEquippedWeapon(MeshComp))
	{
		Weapon->StopWeaponTrail();
	}
}

AMortisWeaponBase* UMortisANS_WeaponTrail::GetEquippedWeapon(USkeletalMeshComponent* MeshComp) const
{
	if (!MeshComp)
		return nullptr;

	AMortisCharacterBase* Owner = Cast<AMortisCharacterBase>(MeshComp->GetOwner());
	if (!Owner)
		return nullptr;

	UMortisCombatComponent* CombatComponent = Owner->GetCombatComponent();

	if (!CombatComponent)
		return nullptr;

	return CombatComponent->GetCharacterCurrentEquippedWeapon();
}
