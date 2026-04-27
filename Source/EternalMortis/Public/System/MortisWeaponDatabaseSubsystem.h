// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/MortisStructTypes.h"
#include "Types/MortisWeaponDataTypes.h"
#include "MortisWeaponDatabaseSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisWeaponDatabaseSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    // UGameInstanceSubsystem Override
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable)
    bool GetWeaponRowByTag(FGameplayTag WeaponTag, FMortisWeaponRow& OutWeaponRow) const;

    UFUNCTION(BlueprintCallable)
    bool GenerateWeaponByTag(FGameplayTag WeaponTag, FMortisWeaponRow& OutWeaponRow) const;

    UFUNCTION(BlueprintCallable)
    bool GenerateRandomWeaponForFloor(int32 Floor, FMortisWeaponRow& OutWeaponRow) const;

    UFUNCTION(BlueprintCallable)
    bool GenerateRandomWeaponByGrade(EMortisWeaponGrade Grade, FMortisWeaponRow& OutWeaponRow) const;

    UFUNCTION(BlueprintCallable)
    bool GetWeaponGradeStyleByHandle(const FDataTableRowHandle& InHandle, FMortisWeaponGradeStyleRow& OutStyleRow) const;

    const FMortisDropRuleRow* GetDropRuleForFloor(int32 Floor) const;

protected:
    void BuildWeaponCaches();
    void BuildDropRuleCaches();

    bool PickRandomGrade(const FMortisGradeWeights& InWeights, EMortisWeaponGrade& OutGrade) const;
    bool HasAnyWeaponInGrade(EMortisWeaponGrade Grade) const;

private:
    UPROPERTY()
    TObjectPtr<UDataTable> WeaponTable = nullptr;

    UPROPERTY()
    TObjectPtr<UDataTable> DropRuleTable = nullptr;

    UPROPERTY()
    TObjectPtr<UDataTable> WeaponGradeStyleTable = nullptr;

    TMap<FGameplayTag, FMortisWeaponRow> WeaponRowsByTag;
    TMap<EMortisWeaponGrade, TArray<FMortisWeaponRow>> WeaponRowsByGrade;
    TMap<int32, FMortisDropRuleRow> DropRulesByFloor;
};
