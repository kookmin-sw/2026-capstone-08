// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/MortisWeaponDataTypes.h"
#include "MortisWeaponInventorySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, const FMortisWeaponRow&, NewWeaponRow);

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisWeaponInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    bool HasWeapon() const { return CurrentWeapon.IsValid(); }

    UFUNCTION(BlueprintCallable)
    const FMortisWeaponRow& GetCurrentWeapon() const { return CurrentWeapon; }

    UFUNCTION(BlueprintCallable)
    void SetCurrentWeapon(const FMortisWeaponRow& InWeaponRow);

    UFUNCTION(BlueprintCallable)
    void ClearCurrentWeapon();

    void SetInitialWeaponTagFromSave();

    FOnWeaponChanged OnWeaponChanged;

private:
    UPROPERTY()
    FMortisWeaponRow CurrentWeapon;

};
