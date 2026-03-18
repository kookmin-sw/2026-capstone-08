// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MortisBTT_RotateToTarget.generated.h"

struct FBTRotateToTargetMemory
{
	TWeakObjectPtr<APawn> OwningPawn;
	TWeakObjectPtr<AActor> TargetActor;

	bool IsValid() const
	{
		return OwningPawn.IsValid() && TargetActor.IsValid();
	}

	void Reset()
	{
		OwningPawn.Reset();
		TargetActor.Reset();
	}
};
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBTT_RotateToTarget : public UBTTaskNode
{
	GENERATED_BODY()

	UMortisBTT_RotateToTarget();

	//~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool HasReachedAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const;
private:
	UPROPERTY(EditAnywhere, Category = "Face Target")
	float AnglePrecision = 10.f;

	UPROPERTY(EditAnywhere, Category = "Face Target")
	float RotationInterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, Category = "Face Target")
	FBlackboardKeySelector TargetToFaceKey;
	
};
