// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionTypes.h"

#include "MortisAIController.generated.h"

class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMortisAIController(const FObjectInitializer& ObjectInitializer);

	//~ Begin IGenericTeamAgentInterface Begin
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//~ End IGenericTeamAgentInterface Begin
	
	void ConfigurePerceptionFromData(const class UMortisEnemyData* EnemyData);

protected:
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	// TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetActorKey = FName("TargetActor");

	//~ Begin AAIController Begin
	virtual void OnPossess(APawn* InPawn) override;
	//~ End AAIController Begin
	
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
