// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/MortisBTS_RandomizeFloat.h"

#include "MortisDebugHelper.h"
#include "BehaviorTree/BlackboardComponent.h"

UMortisBTS_RandomizeFloat::UMortisBTS_RandomizeFloat()
{
	NodeName = TEXT("Randomize Float");

	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	
	FloatKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, FloatKey));
}

void UMortisBTS_RandomizeFloat::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		FloatKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UMortisBTS_RandomizeFloat::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}

void UMortisBTS_RandomizeFloat::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	 if (UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent())
	 {
		 BBComp->SetValueAsFloat(FloatKey.SelectedKeyName, FMath::FRandRange(MinValue, MaxValue));
	 }
}

