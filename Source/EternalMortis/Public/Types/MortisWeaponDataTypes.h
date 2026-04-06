// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "MortisWeaponDataTypes.generated.h"


UENUM(BlueprintType)
enum class EMortisWeaponGrade : uint8
{
    Common      UMETA(DisplayName = "Common"),
    Rare        UMETA(DisplayName = "Rare"),
    Epic        UMETA(DisplayName = "Epic"),
    Legendary   UMETA(DisplayName = "Legendary")
};

USTRUCT(BlueprintType)
struct FMortisWeaponGradeWeights
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
struct FMortisWeaponGradeStyleRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EMortisWeaponGrade Grade = EMortisWeaponGrade::Common;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor GradeColor = FLinearColor::White;
};

USTRUCT(BlueprintType)
struct FMortisWeaponRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag WeaponTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EMortisWeaponGrade Grade = EMortisWeaponGrade::Common;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftClassPtr<class AMortisWeaponBase> WeaponClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FDataTableRowHandle GradeStyleRow;

    bool IsValid() const
    {
        return WeaponTag.IsValid() && !WeaponClass.IsNull();
    }
};

USTRUCT(BlueprintType)
struct FMortisWeaponDropRuleRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Floor = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMortisWeaponGradeWeights GradeWeights;

    bool IsValid() const
    {
        return Floor > 0 && GradeWeights.HasAnyPositiveWeight();
    }
};