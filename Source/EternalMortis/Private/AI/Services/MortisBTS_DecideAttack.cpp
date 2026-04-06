// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/MortisBTS_DecideAttack.h"

#include "MortisDebugHelper.h"
#include "BehaviorTree/BlackboardComponent.h"

UMortisBTS_DecideAttack::UMortisBTS_DecideAttack()
{
	NodeName = TEXT("Decide Attack");

	Interval = 3.f;

	bAttackDecideKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, bAttackDecideKey));
}

void UMortisBTS_DecideAttack::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		bAttackDecideKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UMortisBTS_DecideAttack::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}

void UMortisBTS_DecideAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp)
	{
		return;
	}
	
	if (BBComp->GetValueAsBool(bAttackDecideKey.SelectedKeyName))
	{
		return;
	}
	
	if (FMath::FRandRange(0.f, 1.f) <= AttackProbability)
	{
		BBComp->SetValueAsBool(bAttackDecideKey.SelectedKeyName, true);
	}
}
