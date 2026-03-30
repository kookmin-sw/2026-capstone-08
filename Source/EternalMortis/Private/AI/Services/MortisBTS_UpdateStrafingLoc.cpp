// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/MortisBTS_UpdateStrafingLoc.h"

#include "AIController.h"
#include "MortisDebugHelper.h"
#include "MortisFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/CharacterMovementComponent.h"

UMortisBTS_UpdateStrafingLoc::UMortisBTS_UpdateStrafingLoc()
{
	NodeName = TEXT("Update Strafing Location");
	Interval = 0.5f;

	RunMode = EEnvQueryRunMode::RandomBest25Pct;

	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	StrafingLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, StrafingLocationKey));
	StrafingDistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, StrafingDistanceKey));
}

void UMortisBTS_UpdateStrafingLoc::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		StrafingLocationKey.ResolveSelectedKey(*BBAsset);
		StrafingDistanceKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UMortisBTS_UpdateStrafingLoc::GetStaticDescription() const
{
	return FString::Printf(TEXT("Update %s %s"),
		*StrafingLocationKey.SelectedKeyName.ToString(),
		*GetStaticServiceDescription()
	);
}

void UMortisBTS_UpdateStrafingLoc::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UpdateStrafingLocation(OwnerComp);
}

void UMortisBTS_UpdateStrafingLoc::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UpdateStrafingLocation(OwnerComp);
}

void UMortisBTS_UpdateStrafingLoc::UpdateStrafingLocation(UBehaviorTreeComponent& OwnerComp)
{
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

	float StrafingDistance = BBComp->GetValueAsFloat(StrafingDistanceKey.SelectedKeyName);
	FEnvQueryRequest QueryRequest(QueryTemplate, AIC->GetPawn());
	QueryRequest.SetFloatParam(CircleRadiusParamName, StrafingDistance);
	TWeakObjectPtr WeakThis(this);
	TWeakObjectPtr WeakBBComp(BBComp);
	QueryRequest.Execute(RunMode, FQueryFinishedSignature::CreateLambda([WeakThis, WeakBBComp](TSharedPtr<FEnvQueryResult> QueryResult)
	{
		if (!WeakThis.IsValid() || !WeakBBComp.IsValid())
		{
			return;
		}
		if (!QueryResult->IsSuccessful())
		{
			return;
		}
		FVector NewLocation = QueryResult->GetItemAsLocation(0);		
		WeakBBComp->SetValueAsVector(WeakThis->StrafingLocationKey.SelectedKeyName, NewLocation);
	}));
}
