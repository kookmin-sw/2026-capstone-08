// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/MortisEnumTypes.h"
#include "MortisEnemyData.generated.h"

class AMortisEnemyCharacter;
class UBlackboardData;
class UBehaviorTree;
class UMortisAbilitySetBase;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisEnemyData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<USkeletalMesh> EnemyMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	float CapsuleRadius = 35.f;

	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	float CapsuleHalfHeight = 90.f;

	/* Movement */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FRotator RotationRate = FRotator(0.f, 180.f, 0.f);
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MaxWalkSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BrakingDecelerationWalking = 1000.f;
	
	/* Ability system */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TObjectPtr<UMortisAbilitySetBase> AbilitySet;

	/* AI */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	// UPROPERTY(EditDefaultsOnly, Category = "AI")
	// TObjectPtr<UBlackboardData> BlackboardData;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SightRadius = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float LoseSightRadius = 5500.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float PeripheralVisionAngleDegrees = 360.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Detour Crowd Avoidance Config")
	bool bEnableCrowdAvoidance = true;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Detour Crowd Avoidance Config", meta = (EditCondition = "bEnableCrowdAvoidance", UIMIN = "1", UIMAX = "4"))
	int32 AvoidanceQuality = 4; 

	UPROPERTY(EditDefaultsOnly, Category = "AI|Detour Crowd Avoidance Config", meta = (EditCondition = "bEnableCrowdAvoidance"))
	float AvoidanceQueryRange = 600.f;

	/* Team Agent */
	EMortisTeam Affiliation = EMortisTeam::Enemy; 
};
