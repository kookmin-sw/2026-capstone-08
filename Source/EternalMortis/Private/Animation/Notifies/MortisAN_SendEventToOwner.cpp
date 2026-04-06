// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/MortisAN_SendEventToOwner.h"

#include "AbilitySystemBlueprintLibrary.h"


void UMortisAN_SendEventToOwner::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}
	AActor* Owner = MeshComp->GetOwner();
	if (Owner && EventTag.IsValid())
	{
		FGameplayEventData EventData;
		EventData.Instigator = Owner;
		EventData.Target = Owner;
		EventData.EventTag = EventTag;
	
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, EventData);
	}
}
