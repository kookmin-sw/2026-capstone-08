// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyStates/MortisANS_ActivateInputBuffer.h"
#include "Character/Player/MortisPlayerCharacter.h"

void UMortisANS_ActivateInputBuffer::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	ToggleInputBuffer(MeshComp, true);
}

void UMortisANS_ActivateInputBuffer::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	ToggleInputBuffer(MeshComp, false);
}

void UMortisANS_ActivateInputBuffer::ToggleInputBuffer(USkeletalMeshComponent* MeshComp, bool bEnable)
{
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (AMortisPlayerCharacter* MortisPlayer = Cast<AMortisPlayerCharacter>(Owner))
		MortisPlayer->SetBufferEnabled(bEnable);
}
