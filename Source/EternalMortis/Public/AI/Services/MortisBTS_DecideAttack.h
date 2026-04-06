// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MortisBTS_DecideAttack.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBTS_DecideAttack : public UBTService
{
	GENERATED_BODY()

public:
	UMortisBTS_DecideAttack();
	
	//~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface
	
protected:
	//~ Begin UBTService Interface 
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	//~ End UBTService Interface
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector bAttackDecideKey;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AttackProbability = 0.5f;
};
