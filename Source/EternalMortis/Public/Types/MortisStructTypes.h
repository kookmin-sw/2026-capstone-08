// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayTagContainer.h"
#include "MortisGameplayTags.h"

#include "GameplayTagContainer.h"
#include "MortisStructTypes.generated.h"

class UMortisPlayerLinkedAnimLayer;
class UInputMappingContext;
class UMortisPlayerGameplayAbility;

USTRUCT(BlueprintType)
struct FMortisAbilityInputBinding
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UMortisPlayerGameplayAbility> AbilityToGrant;

	bool IsValid() const;
};

USTRUCT(BlueprintType)
struct FMortisPlayerWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UMortisPlayerLinkedAnimLayer> WeaponAnimLayerToLink;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputMappingContext* WeaponInputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FMortisAbilityInputBinding> DefaultWeaponAbilities;

};

USTRUCT(BlueprintType)
struct FMortisAttackPatternStep
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly)
	float PlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag HitEventTag;
	
	UPROPERTY(EditDefaultsOnly)
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float DelayAfterStep = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	bool bUseMotionWarping = false;

	UPROPERTY(EditDefaultsOnly)
	FName WarpTargetName = TEXT("AttackTarget");

	UPROPERTY(EditDefaultsOnly)
	bool bContinuousWarpUpdate = false;
};

USTRUCT(BlueprintType)
struct FMortisAttackPattern
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FMortisAttackPatternStep> Steps;

	UPROPERTY(EditDefaultsOnly)
	float MinRange = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxRange = 300.f;

	UPROPERTY(EditDefaultsOnly)
	float MinAngle = -180.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxAngle = 180.f;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer RequiredPhases;
	
	UPROPERTY(EditDefaultsOnly)
	float Weight = 1.0f;
};

