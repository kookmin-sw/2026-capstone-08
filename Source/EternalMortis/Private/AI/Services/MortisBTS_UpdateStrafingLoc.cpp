// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/MortisBTS_UpdateStrafingLoc.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"

UMortisBTS_UpdateStrafingLoc::UMortisBTS_UpdateStrafingLoc()
{
	NodeName = TEXT("Update Strafing Location");
	Interval = 0.5f;

	RunMode = EEnvQueryRunMode::RandomBest25Pct;

	StrafingLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, StrafingLocationKey));
}

void UMortisBTS_UpdateStrafingLoc::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		StrafingLocationKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UMortisBTS_UpdateStrafingLoc::GetStaticDescription() const
{
	return FString::Printf(TEXT("Update %s %s"),
		*StrafingLocationKey.SelectedKeyName.ToString(),
		*GetStaticServiceDescription()
	);
}

void UMortisBTS_UpdateStrafingLoc::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC || !AIC->GetPawn())
	{
		return;
	}
	UBlackboardComponent* BBComp = AIC->GetBlackboardComponent();
	
	if (!BBComp || !QueryTemplate)
	{
		return;
	}
	
	FEnvQueryRequest QueryRequest(QueryTemplate, AIC->GetPawn());

	QueryRequest.Execute(RunMode, FQueryFinishedSignature::CreateLambda([this, BBComp, AIC](TSharedPtr<FEnvQueryResult> QueryResult)
	{
		if (!QueryResult->IsSuccessful())
		{
			return;
		}
		FVector NewLocation = QueryResult->GetItemAsLocation(0);
		FVector CurrentLocation = BBComp->GetValueAsVector(StrafingLocationKey.SelectedKeyName);
		
		BBComp->SetValueAsVector(StrafingLocationKey.SelectedKeyName, NewLocation);
	}));
}
