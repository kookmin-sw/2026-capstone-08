// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/MortisRuneDataTypes.h"
#include "MortisRuneDatabaseSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisRuneDatabaseSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    // UGameInstanceSubsystem Override
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    const FMortisRuneSymbolRow* GetRuneSymbolRow(EMortisRuneSymbol SymbolType) const;
    const FMortisRuneSetRow* GetRuneSetRow(FGameplayTag SetTag) const;
    const FMortisRuneDropRuleRow* GetDropRuleForFloor(int32 Floor) const;

    UFUNCTION(BlueprintPure)
    FText GetRuneSetDisplayName(FGameplayTag SetTag) const;

    UFUNCTION(BlueprintCallable)
    FMortisRuneInstance GenerateRune(int32 CurrentFloor) const;

    UFUNCTION(BlueprintCallable)
    FMortisRuneInstance GenerateRuneWithTag(FGameplayTag SetTag, EMortisRuneGrade Grade) const;

    UFUNCTION(BlueprintCallable)
    FMortisRuneInstance GenerateRuneWithTagAndSymbol(FGameplayTag SetTag, EMortisRuneGrade Grade, EMortisRuneSymbol SymbolType) const;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Rune")
    TObjectPtr<UDataTable> RuneSymbolTable;

    UPROPERTY(EditDefaultsOnly, Category = "Rune")
    TObjectPtr<UDataTable> RuneSetTable;

    UPROPERTY(EditDefaultsOnly, Category = "Rune")
    TObjectPtr<UDataTable> DropRuleTable;

    UPROPERTY()
    TMap<EMortisRuneSymbol, FMortisRuneSymbolRow> RuneSymbolMap;

    UPROPERTY()
    TMap<FGameplayTag, FMortisRuneSetRow> RuneSetMap;

    UPROPERTY()
    TArray<FMortisRuneDropRuleRow> DropRules;

    void BuildCaches();

private:
    // *수정 예정* 랜덤하게 뽑기
    template<typename T>
    static const T* GetWeightedRandomEntry(const TArray<T>& Entries, TFunctionRef<int32(const T&)> GetWeight);

    template<typename T>
    static const T* GetUniformRandomEntry(const TArray<T>& Entries);

    static FMortisRuneValueRange GetValueRangeByGrade(const FMortisRuneSymbolRow& SymbolRow, EMortisRuneGrade Grade);
};

template<typename T>
const T* UMortisRuneDatabaseSubsystem::GetWeightedRandomEntry(const TArray<T>& Entries, TFunctionRef<int32(const T&)> GetWeight)
{
    int32 TotalWeight = 0;
    for (const T& Entry : Entries)
    {
        TotalWeight += FMath::Max(0, GetWeight(Entry));
    }

    if (TotalWeight <= 0)
        return nullptr;

    const int32 Roll = FMath::RandRange(1, TotalWeight);
    int32 Acc = 0;

    for (const T& Entry : Entries)
    {
        Acc += FMath::Max(0, GetWeight(Entry));
        if (Roll <= Acc)
            return &Entry;
    }

    return nullptr;
}

template<typename T>
const T* UMortisRuneDatabaseSubsystem::GetUniformRandomEntry(const TArray<T>& Entries)
{
    if (Entries.Num() <= 0)
    {
        return nullptr;
    }

    const int32 Index = FMath::RandRange(0, Entries.Num() - 1);
    return &Entries[Index];
}
