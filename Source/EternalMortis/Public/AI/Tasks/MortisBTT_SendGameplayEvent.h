// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MortisBTT_Base.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MortisBTT_SendGameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBTT_SendGameplayEvent : public UMortisBTT_Base
{
	GENERATED_BODY()

public:
	UMortisBTT_SendGameplayEvent();

	//~ Begin UBTNode Interface 
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	//~ End UBTNode Interface 

	//~ Begin UBTTaskNode Interface
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	//~ End UBTTaskNode Interface

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag StateTagToWait; 

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector EventMagnitude;
};
