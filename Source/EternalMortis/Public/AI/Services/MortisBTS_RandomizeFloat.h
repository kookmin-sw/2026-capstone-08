// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MortisBTS_RandomizeFloat.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBTS_RandomizeFloat : public UBTService
{
	GENERATED_BODY()
	
public:
	UMortisBTS_RandomizeFloat();
	
	//~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

protected:
	//~ Begin UBTService Interface
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//~ End UBTService Interface
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector FloatKey;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float MinValue = 0.f;
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float MaxValue = 1.f;
};
