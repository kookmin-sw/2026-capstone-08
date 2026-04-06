// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EQS/MortisEQC_TargetActor.h"
#include "Controllers/MortisAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UMortisEQC_TargetActor::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);
	
	AActor* OwnerActor = Cast<AActor>(QueryInstance.Owner.Get());
	if (!OwnerActor)
	{
		return;
	}

	UBlackboardComponent* BBComp = UAIBlueprintHelperLibrary::GetBlackboard(OwnerActor);
	if (BBComp)
	{
		if (const AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(MortisBlackboardKeys::TargetActor)))
		{
			UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
		}
	}
}
