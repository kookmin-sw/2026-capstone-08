// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionTypes.h"

#include "MortisAIController.generated.h"

namespace MortisBlackboardKeys
{
	static const FName TargetActor = TEXT("TargetActor");
	static const FName TargetDist = TEXT("TargetDist");
	static const FName StrafingDistance = TEXT("StrafingDistance");
	
}
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

	AActor* GetTargetActor() const;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	//~ Begin AAIController Begin
	virtual void OnPossess(APawn* InPawn) override;
	//~ End AAIController Begin
	
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
