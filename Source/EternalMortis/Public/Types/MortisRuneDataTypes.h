// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "MortisRuneDataTypes.generated.h"

class UGameplayEffect;
class UGameplayAbility;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;

UENUM(BlueprintType)
enum class EMortisRuneGrade : uint8
{
    Common,
    Rare,
    Epic,
    Legendary
};

UENUM(BlueprintType)
enum class EMortisRuneSymbol : uint8
{
    None,
    MaxHealth,
    MaxStamina,
    StaminaHealSpeed,
    MaxMana,
    ManaHealSpeed,
    Strength,
    Dexterity,
    Intelligence,
};

USTRUCT(BlueprintType)
struct FMortisActiveRuneSetStateVariable
{
    GENERATED_BODY()
    
    FMortisActiveRuneSetStateVariable() = default;

    explicit FMortisActiveRuneSetStateVariable(TObjectPtr<UTexture2D> InIcon)
        : Icon(InIcon)
    {
    }
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsPassive = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CurrentStack = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DurationStartTime = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DurationEndTime = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CooldownStartTime = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CooldownEndTime = 0.f;
};

USTRUCT(BlueprintType)
struct FMortisRuneValueRange
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MinValue = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxValue = 0.0f;
};

USTRUCT(BlueprintType)
struct FMortisRuneSetWeight
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag SetTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Weight = 1;
};

USTRUCT(BlueprintType)
struct FMortisRuneGradeWeight
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EMortisRuneGrade Grade;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Weight = 1;
};

USTRUCT(BlueprintType)
struct FMortisRuneInstance
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGuid InstanceId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag SetTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMortisRuneSymbol SymbolType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMortisRuneGrade Grade;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RolledValue = 0.f;
};


USTRUCT(BlueprintType)
struct FMortisActiveRuneSetState
{
    GENERATED_BODY()
    
    // 배열로 하니깐 블루프린트에 노출이 안돼서 Tarray로 변경하고 생성자에 크기 3으로 만듦... 나중에 확인 필요
    /*
    FMortisActiveRuneSetState() 
    {
        Variables.SetNum(3);
    }
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag SetTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText SetName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CurrentCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CurrentLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FMortisActiveRuneSetStateVariable> Variables;
};


USTRUCT(BlueprintType)
struct FMortisSetTierDef
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ActivateCount = 2;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayEffect>> GrantedEffects;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;
};

USTRUCT(BlueprintType)
struct FMortisRuneSymbolRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EMortisRuneSymbol SymbolType;

    // 실제 UI 표시용 문자
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> Glyph;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag StatTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DescriptionFormat;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UGameplayEffect> StatEffect;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMortisRuneValueRange CommonRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMortisRuneValueRange RareRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMortisRuneValueRange EpicRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMortisRuneValueRange LegendaryRange;
};

USTRUCT(BlueprintType)
struct FMortisRuneSetRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag SetTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText SetName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    //TObjectPtr<UTexture2D> Icon = nullptr;
    TArray<TObjectPtr<UTexture2D>> Icon;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<EMortisRuneSymbol> AllowedSymbols;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FMortisSetTierDef> TierDefs;
};

USTRUCT(BlueprintType)
struct FMortisRuneDropRuleRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MinFloor = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MaxFloor = 999;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FMortisRuneSetWeight> SetWeights;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FMortisRuneGradeWeight> GradeWeights;
};
