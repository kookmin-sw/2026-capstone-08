// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MortisBTT_Base.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MortisBTT_SelectAttackPattern.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBTT_SelectAttackPattern : public UMortisBTT_Base
{
	GENERATED_BODY()

public:
	UMortisBTT_SelectAttackPattern();
	
	//~ Begin UBTNode Interface 
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	//~ Begin UBTNode Interface
	
	//~ Begin UBTTaskNode Interface 
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//~ Begin UBTTaskNode Interface	

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector DistToTargetKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AngleToTargetKey;
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AttackPatternIndexKey;

};
