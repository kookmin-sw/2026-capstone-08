// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/MortisEnemyCombatComponent.h"

// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/MortisEnemyCombatComponent.h"

#include "MortisDebugHelper.h"
#include "Character/Enemy/MortisEnemyData.h"
#include "Controllers/MortisAIController.h"

void UMortisEnemyCombatComponent::SetAttackPattern(UMortisAttackPatternData* PatternData)
{
	if (PatternData)
	{
		// MORTIS_LOG("");
		AttackPatterns = PatternData->AttackPatterns;
	}
}

const FMortisAttackPattern* UMortisEnemyCombatComponent::GetAttackPatternByIndex(int32 InIndex) const
{
	if (!AttackPatterns.IsValidIndex(InIndex))
	{
		MORTIS_LOG("%d is Invalid index", InIndex);
		return nullptr;
	}
	return &AttackPatterns[InIndex];
}

int32 UMortisEnemyCombatComponent::SelectAttackPattern(float DistanceToTarget, float AngleToTarget) const
{
	TArray<int32> ValidIndices;
	float TotalWeight = 0.f;
	for (int32 i = 0; i < AttackPatterns.Num(); i++)
	{
		MORTIS_LOG("DistanceToTarget: %f, AngleToTarget: %f, RequiredPhase: %s",
			DistanceToTarget,
			AngleToTarget,
			AttackPatterns[i].RequiredPhases.HasTagExact(CurrentPhase) ? TEXT("true") : TEXT("false"));
		if (DistanceToTarget >= AttackPatterns[i].MinRange &&
			DistanceToTarget <= AttackPatterns[i].MaxRange &&
			AngleToTarget >= AttackPatterns[i].MinAngle &&
			AngleToTarget <= AttackPatterns[i].MaxAngle &&
			AttackPatterns[i].RequiredPhases.HasTagExact(CurrentPhase))
		{
			ValidIndices.Add(i);
			TotalWeight += AttackPatterns[i].Weight;
		}
	}

	float Accumulated = 0.f;
	float RandomValue = FMath::FRandRange(0.f, TotalWeight);
	for (const int32 ValidIndex : ValidIndices)
	{
		Accumulated += AttackPatterns[ValidIndex].Weight;
		if (Accumulated >= RandomValue)
		{
			return ValidIndex;
		}
	}
	return INDEX_NONE;
}
