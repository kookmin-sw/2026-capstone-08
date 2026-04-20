// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyStates/MortisANS_AttackTracing.h"

#include "Character/MortisCharacterBase.h"
#include "Components/Combat/MortisCombatComponent.h"

void UMortisANS_AttackTracing::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (IMortisCombatInterface* CombatInterface = Cast<IMortisCombatInterface>(MeshComp->GetOwner()))
	{
		if (UMortisCombatComponent* CombatComp = CombatInterface->GetCombatComponent())
		{
			CombatComp->BeginAttackTrace(TraceConfig);
		}
	}
}

void UMortisANS_AttackTracing::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	if (IMortisCombatInterface* CombatInterface = Cast<IMortisCombatInterface>(MeshComp->GetOwner()))
	{
		if (UMortisCombatComponent* CombatComp = CombatInterface->GetCombatComponent())
		{
			CombatComp->SetCurrentSlotTag(TraceConfig.SlotTag);
			CombatComp->UpdateAttackTrace();
		}
	}
}

void UMortisANS_AttackTracing::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (IMortisCombatInterface* CombatInterface = Cast<IMortisCombatInterface>(MeshComp->GetOwner()))
	{
		if (UMortisCombatComponent* CombatComp = CombatInterface->GetCombatComponent())
		{
			CombatComp->EndAttackTrace(TraceConfig);
		}
	}
}
