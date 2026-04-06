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

	PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
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
	
	if (EnemyData->bEnableCrowdAvoidance)
	{
		if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
		{
			if (EnemyData->bEnableCrowdAvoidance)
			{
				switch (EnemyData->AvoidanceQuality)
				{
				case 1: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);
					break;
				case 2: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium);
					break;
				case 3: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
					break;
				case 4: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
					break;
				default:
					break;
				}
			}
		
			CrowdComp->SetCrowdSimulationState(EnemyData->bEnableCrowdAvoidance ? ECrowdSimulationState::Enabled : ECrowdSimulationState::ObstacleOnly);
			CrowdComp->SetCrowdAvoidanceQuality(static_cast<ECrowdAvoidanceQuality::Type>(EnemyData->AvoidanceQuality - 1));
			CrowdComp->SetCrowdCollisionQueryRange(EnemyData->AvoidanceQueryRange);
		}
	}

	SetGenericTeamId(static_cast<uint8>(EnemyData->Affiliation));
}

AActor* AMortisAIController::GetTargetActor() const
{
	if (const UBlackboardComponent* BBComp = GetBlackboardComponent())
	{
		return Cast<AActor>(BBComp->GetValueAsObject(MortisBlackboardKeys::TargetActor));
	}
	return nullptr;
}

void AMortisAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AMortisEnemyCharacter* EnemyCharacter = Cast<AMortisEnemyCharacter>(InPawn))
	{
		EnemyCharacter->InitializeEnemyCharacter();
		// MORTIS_LOG("AIController Possess: %s", *InPawn->GetActorNameOrLabel());
		if (const UMortisEnemyData* Data = EnemyCharacter->GetEnemyData())
		{
			ConfigurePerceptionFromData(Data);

			if (Data->BehaviorTree)
			{
				bool Result = RunBehaviorTree(Data->BehaviorTree);
				UBlackboardComponent* BBComp = GetBlackboardComponent();
				if (!BBComp)
				{
					return;
				}
				BBComp->SetValueAsFloat(MortisBlackboardKeys::StrafingDistance, EnemyCharacter->GetRandomStrafingDistance());
				// MORTIS_LOG("Run behavior tree %s", Result ? *FString("Success") : *FString("Fail"));
			}
			
		}
	}
}

void AMortisAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* BBComp = GetBlackboardComponent();
	if (!BBComp)
	{
		return;
	}
	
	if (BBComp->GetValueAsObject(MortisBlackboardKeys::TargetActor))
	{
		return;
	}
	
	if (Actor && Stimulus.WasSuccessfullySensed())
	{
		BBComp->SetValueAsObject(MortisBlackboardKeys::TargetActor, Actor);
		if (GetPawn())
		{
			float Distance = FVector::Dist(GetPawn()->GetActorLocation(), Actor->GetActorLocation());
			BBComp->SetValueAsFloat(MortisBlackboardKeys::TargetDist, Distance);
		}
	}
}
