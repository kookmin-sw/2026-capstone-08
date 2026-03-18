// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "MortisEQC_TargetActor.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisEQC_TargetActor : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	//~ Begin UEnvQueryContext Interface
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
	//~ End UEnvQueryContext Interface
	
};
