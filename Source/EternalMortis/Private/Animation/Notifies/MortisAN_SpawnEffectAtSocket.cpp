// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/MortisAN_SpawnEffectAtSocket.h"

#include "MortisFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Items/Weapons/MortisWeaponBase.h"
#include "Kismet/GameplayStatics.h"

void UMortisAN_SpawnEffectAtSocket::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp)
	{
		return;
	}

	USceneComponent* AttachToComp = MeshComp;
	if (MeshSource == EMortisMeshSource::WeaponMesh)
	{
		if (AMortisWeaponBase* Weapon = UMortisFunctionLibrary::GetWeaponFromCurrentSlot(MeshComp))
		{
			AttachToComp = Weapon->GetWeaponMesh();
		}
	}
	
	if (!MeshComp->DoesSocketExist(SocketName))
	{
		return;
	}
	
	const FVector Location = AttachToComp->GetSocketLocation(SocketName);
	const FRotator Rotation = AttachToComp->GetSocketRotation(SocketName);

	if (NiagaraEffect)
	{
		if (bAttachToSocket)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraEffect, AttachToComp, SocketName,FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
		}
		else
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(AttachToComp->GetWorld(), NiagaraEffect, Location, Rotation);
		}
	}
	else if (CascadeEffect)
	{
		if (bAttachToSocket)
		{
			UGameplayStatics::SpawnEmitterAttached(CascadeEffect, AttachToComp, SocketName);
		}
		else
		{
			UGameplayStatics::SpawnEmitterAtLocation(AttachToComp->GetWorld(), CascadeEffect, Location, Rotation);
		}
	}
}
