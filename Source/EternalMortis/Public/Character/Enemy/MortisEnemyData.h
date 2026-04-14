// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Types/MortisEnumTypes.h"
#include "Types/MortisStructTypes.h"
#include "MortisEnemyData.generated.h"

class UMortisEnemyAbilitySet;
class AMortisEnemyCharacter;
class UBlackboardData;
class UBehaviorTree;
class UMortisAbilitySetBase;
/**
 * 
 */

UCLASS()
class ETERNALMORTIS_API UMortisAttackPatternData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FMortisAttackPattern> AttackPatterns;
};
 
UCLASS()
class ETERNALMORTIS_API UMortisEnemyData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/* Mesh */
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<USkeletalMesh> EnemyMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	FVector MeshScale = FVector::OneVector;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	FRotator MeshRotation = FRotator(0.f, -90.f, 0.f);

	/* Linked Anim Layer */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> LinkedAnimLayerClass;
	
	/* Capsule Component */
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	float CapsuleRadius = 35.f;

	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	float CapsuleHalfHeight = 90.f;

	/* Animation */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> AnimClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float SpeedInterpSpeed = 8.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float DirectionInterpSpeed = 10.f;
	
	/* For Two Hand IK */
	UPROPERTY(EditDefaultsOnly, Category = "Animation|IK")
	bool bUseTwoHandedIK = false;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|IK", meta = (EditCondition = "bUseTwoHandedIK"))
	FName LeftHandSocketName = TEXT("LeftHandGripSocket");
	
	UPROPERTY(EditAnywhere, Category = "AnimData|IK")
	FName LeftUpperArmSocketName = TEXT("upperarm_l");
	
	UPROPERTY(EditAnywhere, Category = "AnimData|IK")
	float ElbowOutOffset = 50.f;

	UPROPERTY(EditAnywhere, Category = "AnimData|IK")
	float ElbowBackOffset = 30.f;
	
	/* Combat */
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FMortisWeaponCommonData UnarmedData;
	
	/* Movement */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FRotator RotationRate = FRotator(0.f, 180.f, 0.f);

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float IdleMaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float StrafingMaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ChasingMaxWalkSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BrakingDecelerationWalking = 1000.f;
	
	/* Ability system */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TObjectPtr<UMortisEnemyAbilitySet> AbilitySet;

	/* AI */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	/* EQS */
	UPROPERTY(EditDefaultsOnly, Category = "AI|EQS")
	TMap<FGameplayTag, FMortisDistanceRange> PhaseStrafingRanges; 
	
	/* Perception */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float SightRadius = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float LoseSightRadius = 5500.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float PeripheralVisionAngleDegrees = 360.f;

	/* Detour Crowd Avoidance */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Detour Crowd Avoidance Config")
	bool bEnableCrowdAvoidance = true;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Detour Crowd Avoidance Config", meta = (EditCondition = "bEnableCrowdAvoidance", UIMIN = "1", UIMAX = "4"))
	int32 AvoidanceQuality = 4; 

	UPROPERTY(EditDefaultsOnly, Category = "AI|Detour Crowd Avoidance Config", meta = (EditCondition = "bEnableCrowdAvoidance"))
	float AvoidanceQueryRange = 600.f;

	/* Team Agent */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	EMortisTeam Affiliation = EMortisTeam::Enemy;

	/* Attack Pattern */
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UMortisAttackPatternData> AttackPatternData;
	
};
