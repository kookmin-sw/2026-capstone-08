// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/MortisAIController.h"
#include "MortisDebugHelper.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Character/Enemy/MortisEnemyData.h"
#include "Types/MortisEnumTypes.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AMortisAIController::AMortisAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

	PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::HandleTargetPerceptionUpdated);
}

ETeamAttitude::Type AMortisAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(&Other))
	{
		if (GetGenericTeamId() == OtherTeamAgent->GetGenericTeamId())
		{
			return ETeamAttitude::Friendly;
		}
		if (GetGenericTeamId() != OtherTeamAgent->GetGenericTeamId()
			&& OtherTeamAgent->GetGenericTeamId() != static_cast<uint8>(EMortisTeam::Neutral))
		{
			return ETeamAttitude::Hostile;
		}
	}
	// return ETeamAttitude::Neutral;
	return ETeamAttitude::Hostile;
}

void AMortisAIController::ConfigurePerceptionFromData(const UMortisEnemyData* EnemyData)
{
	if (!EnemyData || !SightConfig)
	{
		return;
	}
	
	SightConfig->SightRadius = EnemyData->SightRadius;
	SightConfig->LoseSightRadius = EnemyData->LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = EnemyData->PeripheralVisionAngleDegrees;
	
	PerceptionComponent->ConfigureSense(*SightConfig);

	if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		CrowdComp->SetCrowdSimulationState(EnemyData->bEnableCrowdAvoidance? ECrowdSimulationState::Enabled : ECrowdSimulationState::ObstacleOnly);
		CrowdComp->SetCrowdAvoidanceQuality(static_cast<ECrowdAvoidanceQuality::Type>(EnemyData->AvoidanceQuality - 1));
		CrowdComp->SetCrowdCollisionQueryRange(EnemyData->AvoidanceQueryRange);
	}

	SetGenericTeamId(static_cast<uint8>(EnemyData->Affiliation));
}

void AMortisAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AMortisEnemyCharacter* EnemyCharacter = Cast<AMortisEnemyCharacter>(InPawn))
	{
		// MORTIS_LOG("AIController Possess: %s", *InPawn->GetActorNameOrLabel());
		if (const UMortisEnemyData* Data = EnemyCharacter->GetEnemyData())
		{
			ConfigurePerceptionFromData(Data);

			if (Data->BehaviorTree)
			{
				bool Result = RunBehaviorTree(Data->BehaviorTree);
				// MORTIS_LOG("Run behavior tree %s", Result ? *FString("Success") : *FString("Fail"));
			}
			
		}
	}
}

void AMortisAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (UBlackboardComponent* BBComp = GetBlackboardComponent())
	{
		if (!BBComp->GetValueAsObject(TargetActorKey))
		{
			if (Actor && Stimulus.WasSuccessfullySensed())
			{
				BBComp->SetValueAsObject(TargetActorKey, Actor);
				// MORTIS_LOG("Set TargetActor: %s", *Actor->GetActorNameOrLabel());
			}
		}
	}
}
