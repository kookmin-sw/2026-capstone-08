// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyStates/MortisANS_ApplyTag.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

void UMortisANS_ApplyTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp)
		return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
		return;

	const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner);
	if (!ASI)
		return;

	if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
	{
		if (TagToGrant.IsValid())
			ASC->AddLooseGameplayTag(TagToGrant);
	}
}

void UMortisANS_ApplyTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
		return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
		return;

	const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner);
	if (!ASI)
		return;

	if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
	{
		if (TagToGrant.IsValid())
			ASC->RemoveLooseGameplayTag(TagToGrant);
	}
}
