// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "MortisBTS_UpdateStrafingLoc.generated.h"

class UEnvQuery;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBTS_UpdateStrafingLoc : public UBTService
{
	GENERATED_BODY()

public:
	UMortisBTS_UpdateStrafingLoc();

	//~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface
	
protected:
	//~ Begin UBTService Interface 
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	//~ End UBTService Interface 

	UPROPERTY(EditAnywhere, Category = "AI|EQS")
	TObjectPtr<UEnvQuery> QueryTemplate;

	UPROPERTY(EditAnywhere, Category = "AI|EQS")
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode;

	UPROPERTY(EditAnywhere, Category = "AI|EQS")
	FBlackboardKeySelector StrafingLocationKey;
};
