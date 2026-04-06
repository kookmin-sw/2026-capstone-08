// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MortisBTS_UpdateTargetContext.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBTS_UpdateTargetContext : public UBTService
{
	GENERATED_BODY()

public:
	UMortisBTS_UpdateTargetContext();

	//~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface
	
protected:
	//~ Begin UBTService Interface
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	//~ End UBTService Interface 

private:
	void UpdateTargetContext(UBlackboardComponent* BBComp, const AActor* OwnerActor, const AActor* TargetActor);
	
protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DistToTargetKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AngleToTargetKey;
};
