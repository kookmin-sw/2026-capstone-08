// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MortisBTT_Strafe.generated.h"

struct FBTStrafeMemory
{
	FVector LastStrafingLocation;
	float ElapsedTime;
};
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBTT_Strafe : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMortisBTT_Strafe();

	//~ Begin UBTNode Interface 
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	virtual uint16 GetInstanceMemorySize() const override;
	//~ Begin UBTNode Interface
	
	//~ Begin UBTTaskNode Interface 
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//~ Begin UBTTaskNode Interface

protected:
	UPROPERTY(EditAnywhere, Category = "Strafing")
	FBlackboardKeySelector StrafingLocationKey;

	UPROPERTY(EditAnywhere, Category = "Strafing")
	float AcceptableRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = "Strafing")
	float UpdateInterval = 2.f;
};
