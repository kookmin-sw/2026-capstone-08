// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "MortisGameplayTags.h"
#include "MortisRunStateSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int32, DeltaGold);

UCLASS(BlueprintType, Blueprintable)
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
    int32 GetCurrentConcept() const { return CurrentConcept; }

    UFUNCTION(BlueprintPure, Category = "Mortis|RunState")
    int32 GetCurrentRoomIndex() const { return CurrentRoomIndex; }

    UFUNCTION(BlueprintPure, Category = "Mortis|RunState")
    int32 GetClearedRoomCount() const { return ClearedRoomCount; }

    UFUNCTION(BlueprintPure, Category = "Mortis|RunState")
    bool CanSeeTrueEnding() const { return bCanSeeTrueEnding; }

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
    void SetCurrentConcept(int32 NewConcept);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void AdvanceConcept();

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void SetCurrentRoomIndex(int32 NewRoomIndex);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void AddClearedRoomCount(int32 Amount = 1);

    // ----- Revive -----
    UFUNCTION(BlueprintPure, Category = "Mortis|RunState|Revive")
    int32 GetCurrentReviveCost() const { return CurrentReviveCost; }

    UFUNCTION(BlueprintPure, Category = "Mortis|RunState|Revive")
    bool CanAffordRevive() const;

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState|Revive")
    bool TrySpendReviveCost();

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState|Revive")
    void ResetReviveCost();

    // ----- Run -----
    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void SetCurrentWeaponTag(FGameplayTag InWeaponTag);

    UFUNCTION(BlueprintPure, Category = "Mortis|RunState")
    FGameplayTag GetCurrentWeaponTag() { return CurrentWeapon; }

    // ----- Run -----
    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void ResetRunState();

    UFUNCTION(BlueprintCallable, Category = "Mortis|RunState")
    void SetTrueEndingEnabled(bool bTrueEnding);
    
    // ----- Delegate -----
    UPROPERTY(BlueprintAssignable, BlueprintCallable)
    FOnGoldChanged OnGoldChanged;


protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState")
    int32 CurrentGold = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState")
    int32 CurrentFloor = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState")
    int32 CurrentConcept = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|RunState|Revive")
    int32 BaseReviveCost = 100;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|RunState|Revive")
    int32 ReviveCostMultiplier = 2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState|Revive")
    int32 CurrentReviveCost = 100;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState")
    int32 CurrentRoomIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState")
    int32 ClearedRoomCount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState")
    bool bCanSeeTrueEnding = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RunState")
    FGameplayTag CurrentWeapon = MortisGameplayTags::Data_Weapon_DarkIronSword;
};