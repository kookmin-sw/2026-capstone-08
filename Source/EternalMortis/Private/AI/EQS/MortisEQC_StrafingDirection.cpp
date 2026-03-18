// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EQS/MortisEQC_StrafingDirection.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Direction.h"

void UMortisEQC_StrafingDirection::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	if (!QueryOwner)
	{
		return;
	}
	
	UBlackboardComponent* BBComp = UAIBlueprintHelperLibrary::GetBlackboard(QueryOwner);
	if (BBComp)
	{
		FVector StrafingLocation = BBComp->GetValueAsVector(StrafingLocationKeyName);
		if (StrafingLocation.IsNearlyZero())
		{
			// if StrafingLocation is not set
			UEnvQueryItemType_Direction::SetContextHelper(ContextData, QueryOwner->GetActorForwardVector());
			return;
		}
		FVector StrafingDirection = StrafingLocation - QueryOwner->GetActorLocation();
		StrafingDirection.Z = 0.f;
		if (StrafingDirection.IsNearlyZero())
		{
			UEnvQueryItemType_Direction::SetContextHelper(ContextData, QueryOwner->GetActorForwardVector());
			return;
		}
		
		UEnvQueryItemType_Direction::SetContextHelper(ContextData, StrafingDirection.GetSafeNormal());
	}
}
