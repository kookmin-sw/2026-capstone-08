// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/MortisStructTypes.h"
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
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable)
    FMortisRuneInstance GenerateRune(int32 CurrentFloor) const;
    UFUNCTION(BlueprintCallable)
    FMortisRuneInstance GenerateRuneWithTag(FGameplayTag SetTag, EMortisRuneGrade Grade) const;
    UFUNCTION(BlueprintCallable)
    FMortisRuneInstance GenerateRandomRuneWithTag(FGameplayTag SetTag, int32 CurrentFloor) const;
    UFUNCTION(BlueprintCallable)
    FMortisRuneInstance GenerateRuneWithTagAndSymbol(FGameplayTag SetTag, EMortisRuneGrade Grade, EMortisRuneSymbol SymbolType) const;
    UFUNCTION(BlueprintCallable)
    bool GetRuneGradeStyleByHandle(const FDataTableRowHandle& InHandle, FMortisRuneGradeStyleRow& OutStyleRow) const;

    const FMortisRuneSymbolRow* GetRuneSymbolRow(EMortisRuneSymbol SymbolType) const;
    const FMortisRuneSetRow* GetRuneSetRow(FGameplayTag SetTag) const;
    FText GetRuneSetDisplayName(FGameplayTag SetTag) const;

private:
    void BuildCaches();
    const FMortisDropRuleRow* GetDropRuleForFloor(int32 Floor) const;
    static FMortisRuneValueRange GetValueRangeByGrade(const FMortisRuneSymbolRow& SymbolRow, EMortisRuneGrade Grade);

    bool PickRandomGrade(const FMortisGradeWeights& InWeights, EMortisRuneGrade& OutGrade) const;

private:
    UPROPERTY()
    TObjectPtr<UDataTable> RuneSymbolTable = nullptr;

    UPROPERTY()
    TObjectPtr<UDataTable> RuneSetTable = nullptr;

    UPROPERTY()
    TObjectPtr<UDataTable> DropRuleTable = nullptr;

    UPROPERTY()
    TObjectPtr<UDataTable> RuneGradeStyleTable = nullptr;

    UPROPERTY()
    TMap<EMortisRuneSymbol, FMortisRuneSymbolRow> RuneSymbolMap;

    UPROPERTY()
    TMap<FGameplayTag, FMortisRuneSetRow> RuneSetMap;

    UPROPERTY()
    TMap<int32, FMortisDropRuleRow> DropRulesByFloor;

    UPROPERTY()
    TArray<FGameplayTag> ValidRuneSetTags;
};