// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayTagContainer.h"
#include "MortisGameplayTags.h"

#include "GameplayTagContainer.h"
#include "MortisEnumTypes.h"
#include "MortisStructTypes.generated.h"

class UGameplayEffect;
class UMortisPlayerLinkedAnimLayer;
class UInputMappingContext;
class UMortisPlayerGameplayAbility;
class UAnimMontage;
enum class EMortisStatGrade : uint8;
enum class EMortisShopTransactionType : uint8;

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
struct FMortisWeaponCommonData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WeaponDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PoiseDamage;
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WeaponDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PoiseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EMortisStatGrade StrGrade;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EMortisStatGrade DexGrade;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EMortisStatGrade IntGrade;
};

USTRUCT(BlueprintType)
struct FMortisEnemyWeaponData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FMortisWeaponCommonData CommonData;
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
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float DelayAfterStep = 0.0f;

	/* Motion Warping */
	UPROPERTY(EditDefaultsOnly)
	bool bUseMotionWarping = false;

	UPROPERTY(EditDefaultsOnly)
	FName WarpTargetName = TEXT("AttackTarget");

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bUseMotionWarping"))
	EMortisWarpTargetMode WarpTargetMode = EMortisWarpTargetMode::ActorLocation;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bUseMotionWarping"))
	float DesiredDistance = 100.f;
	
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bUseMotionWarping"))
	bool bContinuousWarpUpdate = false;

	/* Combo */
	UPROPERTY(EditDefaultsOnly)
	bool bUseComboTransitionNotify = false;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bUseComboTransitionNotify"))
	FGameplayTag ComboTransitionEventTag = MortisGameplayTags::Event_Combat_Combo_Next;
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

USTRUCT(BlueprintType)
struct FMortisTagToBoolMapping
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mappping")
	FGameplayTag Tag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mappping")
	FName BoolPropertyName;
};

// for EQS
USTRUCT(BlueprintType)
struct FMortisDistanceRange
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Distance")
	float MinDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Distance")
	float MaxDistance;
};

// 상점 아이템용
USTRUCT(BlueprintType)
struct FMortisShopItemState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	int32 Price = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	bool bCanSteal = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	bool bSold = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	bool bStolen = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	EMortisShopTransactionType PendingTransaction;

	void ResetRuntimeState()
	{
		bSold = false;
		bStolen = false;
	}
};

USTRUCT(BlueprintType)
struct FMortisAttackTraceConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float Radius = 5.f;
	
	UPROPERTY(EditDefaultsOnly)
	FName StartSocket;

	UPROPERTY(EditDefaultsOnly)
	FName EndSocket;

	UPROPERTY(EditDefaultsOnly)
	EMortisMeshSource MeshSource = EMortisMeshSource::WeaponMesh;
	
	bool operator==(const FMortisAttackTraceConfig& Config) const
	{
		return StartSocket == Config.StartSocket && EndSocket == Config.EndSocket && MeshSource == Config.MeshSource;
	}
};

USTRUCT(BlueprintType)
struct FWeaponAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> RecoveryMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StaminaCost = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ManaCost = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackCoef = 1.0f;
};

USTRUCT(BlueprintType)
struct FMortisGradeWeights
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Common = 70.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Rare = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Epic = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Legendary = 2.f;

	bool HasAnyPositiveWeight() const
	{
		return Common > 0.f || Rare > 0.f || Epic > 0.f || Legendary > 0.f;
	}
};

USTRUCT(BlueprintType)
struct FMortisDropRuleRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Floor = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMortisGradeWeights WeaponGradeWeights;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMortisGradeWeights RuneGradeWeights;

	bool IsValid() const
	{
		return Floor > 0
			&& WeaponGradeWeights.HasAnyPositiveWeight()
			&& RuneGradeWeights.HasAnyPositiveWeight();
	}
};