// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/MortisBTS_ApplyStateGE.h"

#include "GameplayEffectTypes.h"
#include "MortisDebugHelper.h"
#include "MortisFunctionLibrary.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"

UMortisBTS_ApplyStateGE::UMortisBTS_ApplyStateGE()
{
	NodeName = TEXT("Apply Effect Gameplay Effect");

	INIT_SERVICE_NODE_NOTIFY_FLAGS();
}

uint16 UMortisBTS_ApplyStateGE::GetInstanceMemorySize() const
{
	return sizeof(FBTApplyStateGEMemory);
}

FString UMortisBTS_ApplyStateGE::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}

void UMortisBTS_ApplyStateGE::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	FBTApplyStateGEMemory* Memory = CastInstanceNodeMemory<FBTApplyStateGEMemory>(NodeMemory);
	if (!Memory)
	{
		return;
	}
	
	if (UMortisAbilitySystemComponent* ASC = UMortisFunctionLibrary::GetMortisASC(OwnerComp))
	{
		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(StateGEClass, 1.f, EffectContextHandle);

		if (SpecHandle.IsValid())
		{
			Memory->ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}	
}

void UMortisBTS_ApplyStateGE::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	FBTApplyStateGEMemory* Memory = CastInstanceNodeMemory<FBTApplyStateGEMemory>(NodeMemory);
	if (!Memory)
	{
		return;
	}
	
	if (UMortisAbilitySystemComponent* ASC = UMortisFunctionLibrary::GetMortisASC(OwnerComp))
	{
		if (Memory->ActiveHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Memory->ActiveHandle);
			Memory->ActiveHandle.Invalidate();
		}
	}	
}
