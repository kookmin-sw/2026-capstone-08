// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MortisRunStateSubsystem.h"
#include "System/MortisMetaProgressionSubsystem.h"
#include "Kismet/GameplayStatics.h"

bool UMortisRunStateSubsystem::HasEnoughGold(int32 Amount) const
{
    return Amount <= 0 || CurrentGold >= Amount;
}

bool UMortisRunStateSubsystem::SpendGold(int32 Amount)
{
    if (Amount <= 0)
        return true;

    if (!HasEnoughGold(Amount))
        return false;

    CurrentGold -= Amount;
    OnGoldChanged.Broadcast(-Amount);
    return true;
}

void UMortisRunStateSubsystem::AddGold(int32 Amount)
{
    if (Amount <= 0)
        return;

    CurrentGold += Amount;
    OnGoldChanged.Broadcast(Amount);
}

void UMortisRunStateSubsystem::SetGold(int32 NewGold)
{
    CurrentGold = FMath::Max(0, NewGold);
}

void UMortisRunStateSubsystem::SetCurrentFloor(int32 NewFloor)
{
    CurrentFloor = FMath::Max(1, NewFloor);
    ResetReviveCost();
}

void UMortisRunStateSubsystem::AdvanceFloor()
{
    ++CurrentFloor;
    CurrentRoomIndex = 0;
    ResetReviveCost();
}

void UMortisRunStateSubsystem::SetCurrentRoomIndex(int32 NewRoomIndex)
{
    CurrentRoomIndex = FMath::Max(0, NewRoomIndex);
}

void UMortisRunStateSubsystem::AddClearedRoomCount(int32 Amount)
{
    if (Amount <= 0)
        return;

    ClearedRoomCount += Amount;
}

bool UMortisRunStateSubsystem::CanAffordRevive() const
{
    const UGameInstance* GI = GetGameInstance();
    if (!GI)
        return false;

    const UMortisMetaProgressionSubsystem* MetaProgression =
        GI->GetSubsystem<UMortisMetaProgressionSubsystem>();

    if (!MetaProgression)
        return false;

    return MetaProgression->GetMemoryFragments() >= CurrentReviveCost;
}

bool UMortisRunStateSubsystem::TrySpendReviveCost()
{
    UGameInstance* GI = GetGameInstance();
    if (!GI)
        return false;

    UMortisMetaProgressionSubsystem* MetaProgression = GI->GetSubsystem<UMortisMetaProgressionSubsystem>();

    if (!MetaProgression)
        return false;

    if (MetaProgression->GetMemoryFragments() < CurrentReviveCost)
        return false;

    MetaProgression->AddMemoryFragments(-CurrentReviveCost);
    if (ReviveCostMultiplier > 1)
    {
        if (CurrentReviveCost > MAX_int32 / ReviveCostMultiplier)
        {
            CurrentReviveCost = MAX_int32;
        }
        else
        {
            CurrentReviveCost *= ReviveCostMultiplier;
        }
    }

    return true;
}

void UMortisRunStateSubsystem::ResetReviveCost()
{
    CurrentReviveCost = FMath::Max(0, BaseReviveCost);
}

void UMortisRunStateSubsystem::SetCurrentWeaponTag(FGameplayTag InWeaponTag)
{
    CurrentWeapon = InWeaponTag;
}

void UMortisRunStateSubsystem::ResetRunState()
{
    CurrentGold = 0;
    CurrentFloor = 1;
    CurrentRoomIndex = 0;
    ClearedRoomCount = 0;
    bCanSeeTrueEnding = false;
    ResetReviveCost();
    CurrentWeapon = MortisGameplayTags::Data_Weapon_DarkIronSword;
}

void UMortisRunStateSubsystem::SetTrueEndingEnabled(bool bTrueEnding)
{
    bCanSeeTrueEnding = bTrueEnding;
}
