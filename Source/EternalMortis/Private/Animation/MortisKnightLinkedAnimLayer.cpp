// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MortisKnightLinkedAnimLayer.h"

#include "MortisDebugHelper.h"
#include "Character/MortisCharacterBase.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Components/Combat/MortisCombatComponent.h"

void UMortisKnightLinkedAnimLayer::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	OwningCharacter = Cast<AMortisEnemyCharacter>(TryGetPawnOwner());
}

void UMortisKnightLinkedAnimLayer::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	UpdateIKAlpha(DeltaSeconds);
	UpdateLeftElbowJointTarget();
}

void UMortisKnightLinkedAnimLayer::UpdateIKAlpha(float DeltaSeconds)
{
	if (!OwningCharacter)
	{
		return;
	}
	
	UMortisCombatComponent* CombatComp = OwningCharacter->GetCombatComponent();
	if (!CombatComp)
	{
		MORTIS_LOG("CombatComp is null");
		return;
	}

	AMortisWeaponBase* Weapon = CombatComp->GetCurrentWeapon();
	if (!Weapon)
	{
		MORTIS_LOG("Weapon is null");
		return;
	}
		
	UMeshComponent* WeaponMesh = Weapon->GetWeaponMesh();
	if (!WeaponMesh)
	{
		return;
	}

	float TargetAlpha = 0.f;
	if (WeaponMesh->DoesSocketExist(LeftHandGripSocketName))
	{
		FTransform SocketWorldTransform = WeaponMesh->GetSocketTransform(LeftHandGripSocketName, RTS_World);
		FTransform MeshComponentTransform = OwningCharacter->GetMesh()->GetComponentTransform();

		LeftHandIKTarget = SocketWorldTransform.GetRelativeTransform(MeshComponentTransform);
		
		TargetAlpha = 1.f;
	}
	
	CurrentIKAlpha = FMath::FInterpTo(CurrentIKAlpha, TargetAlpha, DeltaSeconds, 10.f);
}

void UMortisKnightLinkedAnimLayer::UpdateLeftElbowJointTarget()
{
	if (!OwningCharacter)
	{
		return;
	}

	if (USkeletalMeshComponent* Mesh = OwningCharacter->GetMesh())
	{
		FTransform ShoulderTransform = Mesh->GetSocketTransform(LeftUpperArmSocketName, RTS_Component);
		
		FTransform MeshTransform = Mesh->GetComponentTransform();
		FVector CompLeft = MeshTransform.InverseTransformVector(OwningCharacter->GetActorRightVector() * -1.f);
		FVector CompBack = MeshTransform.InverseTransformVector(OwningCharacter->GetActorForwardVector() * -1.f);
		FVector ShoulderLoc = ShoulderTransform.GetLocation();
		
		LeftElbowJointTarget = ShoulderLoc + ElbowOutOffset * CompLeft + ElbowBackOffset * CompBack;
	}
}
