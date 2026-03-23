// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "MortisGameplayTags.h"
#include "Types/MortisRuneDataTypes.h"
#include "ActiveGameplayEffectHandle.h"
#include "MortisEquipmentComponent.generated.h"

/**
 * 
 */

class UMortisRuneDatabaseSubsystem;
class UMortisRuneInventorySubsystem;
class UMortisAbilitySystemComponent;
struct FGameplayAbilitySpecHandle;

USTRUCT()
struct FMortisEquippedRuneRuntime
{
    GENERATED_BODY()

    UPROPERTY()
    bool bActivated = false;

    UPROPERTY()
    FGuid RuneInstanceId;

    UPROPERTY()
    FMortisRuneInstance Rune;

    UPROPERTY()
    FActiveGameplayEffectHandle EffectHandle;
};

USTRUCT()
struct FMortisAppliedRuneSetRuntime
{
    GENERATED_BODY()

    UPROPERTY()
    FGameplayTag SetTag;

    UPROPERTY()
    int32 AppliedLevel = 0;

    UPROPERTY()
    TArray<FActiveGameplayEffectHandle> EffectHandles;

    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> AbilityHandles;
};

UCLASS()
class ETERNALMORTIS_API UMortisEquipmentComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()
	
protected:
	// UActorComponent Override
	virtual void BeginPlay() override;

private:
    UPROPERTY()
	UMortisRuneDatabaseSubsystem* RuneDB = nullptr;
    UPROPERTY()
	UMortisRuneInventorySubsystem* RuneInv = nullptr;
    UPROPERTY()
    UMortisAbilitySystemComponent* ASC = nullptr;


    UPROPERTY()
    TArray<FMortisEquippedRuneRuntime> EquippedRuneRuntimes;
    UPROPERTY()
    TMap<FGameplayTag, FMortisAppliedRuneSetRuntime> AppliedRuneSetRuntimes;

    UFUNCTION()
	void ApplyRuneEffect(int32 SlotIndex, const FMortisRuneInstance& RuneToAdd);
    UFUNCTION()
    void RemoveRuneEffect(int32 SlotIndex, const FMortisRuneInstance& RuneToAdd);
    UFUNCTION()
    void UpdateRuneSetBonus(const TArray<FGameplayTag>& ActiveSetTags);

    const FMortisSetTierDef* FindTierDefByLevel(const FMortisRuneSetRow& SetRow, int32 Level) const;

    void ApplySetTier(const FMortisActiveRuneSetState& SetState, const FMortisSetTierDef& TierDef);
    void RemoveSetTier(const FGameplayTag& SetTag);
};
