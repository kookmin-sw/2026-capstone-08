// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MortisRunStateSubsystem.h"

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
}

void UMortisRunStateSubsystem::AdvanceFloor()
{
    ++CurrentFloor;
    CurrentRoomIndex = 0;
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

void UMortisRunStateSubsystem::SetMaxLife(int32 NewMaxLife, bool bClampCurrentLife)
{
    MaxLife = FMath::Max(1, NewMaxLife);

    if (bClampCurrentLife)
        CurrentLife = FMath::Clamp(CurrentLife, 0, MaxLife);
}

void UMortisRunStateSubsystem::SetCurrentLife(int32 NewCurrentLife)
{
    CurrentLife = FMath::Clamp(NewCurrentLife, 0, MaxLife);
}

bool UMortisRunStateSubsystem::LoseLife(int32 Amount)
{
    if (Amount <= 0)
        return false;

    const int32 PrevLife = CurrentLife;
    CurrentLife = FMath::Clamp(CurrentLife - Amount, 0, MaxLife);

    if (PrevLife != CurrentLife)
        return true;

    return false;
}

void UMortisRunStateSubsystem::GainLife(int32 Amount)
{
    if (Amount <= 0)
        return;

    CurrentLife = FMath::Clamp(CurrentLife + Amount, 0, MaxLife);
}

void UMortisRunStateSubsystem::ResetRunState()
{
    CurrentGold = 0;
    CurrentFloor = 1;
    CurrentLife = MaxLife;
    CurrentRoomIndex = 0;
    ClearedRoomCount = 0;

}