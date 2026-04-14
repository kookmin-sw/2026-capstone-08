// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MortisRunStateSubsystem.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ETERNALMORTIS_API UMortisRunStateSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // ----- Getter -----
    UFUNCTION(BlueprintPure, Category = "Mortis|RunState")
    int32 GetCurrentGold() const { return CurrentGold; }

    UFUNCTION(BlueprintPure, Category = "Mortis|RunState")
    int32 GetCurrentFloor() const { return CurrentFloor; }

    UFUNCTION(BlueprintPure, Category = "Mortis|RunState")
    int32 GetCurrentLife() const { return CurrentLife; }

    UFUNCTION(BlueprintPure, Category = "Mortis|RunState")
    int32 GetMaxLife() const { return MaxLife; }

    UFUNCTION(BlueprintPure, Category = "Mortis|RunState")
    int32 GetCurrentRoomIndex() const { return CurrentRoomIndex; }

    UFUNCTION(BlueprintPure, Category = "Mortis|RunState")
    int32 GetClearedRoomCount() const { return ClearedRoomCount; }

    UFUNCTION(BlueprintPure, Category = "Mortis|RunState")
    bool IsRunFailed() const { return CurrentLife <= 0; }

    // ----- Gold -----
    UFUNCTION(BlueprintPure, Category = "Mortis|RunState")
    bool HasEnoughGold(int32 Amount) const;

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    bool SpendGold(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void AddGold(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void SetGold(int32 NewGold);

    // ----- Floor / Room -----
    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void SetCurrentFloor(int32 NewFloor);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void AdvanceFloor();

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void SetCurrentRoomIndex(int32 NewRoomIndex);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void AddClearedRoomCount(int32 Amount = 1);

    // ----- Life -----
    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void SetMaxLife(int32 NewMaxLife, bool bClampCurrentLife = true);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void SetCurrentLife(int32 NewCurrentLife);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    bool LoseLife(int32 Amount = 1);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void GainLife(int32 Amount = 1);

    // ----- Run -----
    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void ResetRunState();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState")
    int32 CurrentGold = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState")
    int32 CurrentFloor = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState")
    int32 CurrentLife = 3;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|RunState")
    int32 MaxLife = 3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState")
    int32 CurrentRoomIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState")
    int32 ClearedRoomCount = 0;
};