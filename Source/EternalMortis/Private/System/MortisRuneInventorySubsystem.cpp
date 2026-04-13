// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MortisRuneInventorySubsystem.h"
#include "System/MortisRuneDatabaseSubsystem.h"

#include "MortisDebugHelper.h"

void UMortisRuneInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 추후에 세이브파일로 불러오도록 할 것
    EquippedRunes.SetNum(SlotCount);
}

bool UMortisRuneInventorySubsystem::AddRune(const FMortisRuneInstance& NewRune)
{
    if (!NewRune.InstanceId.IsValid()) return false;

    OwningRunes.Add(NewRune);
    OnOwningRuneAdded.Broadcast(NewRune);

    return true;
}

bool UMortisRuneInventorySubsystem::EquipRune(int32 SlotIndex, const FMortisRuneInstance RuneInstance)
{
    if (SlotIndex < 0 || SlotIndex >= SlotCount) return false;

    // 람다가 답인가
    int32 InventoryIndex = OwningRunes.IndexOfByPredicate([&](const FMortisRuneInstance& Rune) { return Rune.InstanceId == RuneInstance.InstanceId; });
    if (InventoryIndex == INDEX_NONE) return false;

    // 장착할 룬 인벤에서 제거
    OwningRunes.RemoveAt(InventoryIndex);

    // 룬 장착 및 세트효과 갱신
    EquippedRunes[SlotIndex] = RuneInstance;
    UpdateSetCount(EquippedRunes[SlotIndex].SetTag, 1);

    OnEquippedRuneAdded.Broadcast(SlotIndex, RuneInstance);
    OnOwningRuneRemoved.Broadcast(RuneInstance);
    OnActivatedRuneSetsChanged.Broadcast(GetActiveRuneSets());

    return true;
}

bool UMortisRuneInventorySubsystem::UnequipRune(int32 SlotIndex)
{
    if (SlotIndex < 0 || SlotIndex >= SlotCount) return false;
    if (!EquippedRunes[SlotIndex].InstanceId.IsValid()) return false;

    // 룬 인벤토리로 이동 및 세트효과 갱신
    OwningRunes.Add(EquippedRunes[SlotIndex]);
    UpdateSetCount(EquippedRunes[SlotIndex].SetTag, -1);

    OnOwningRuneAdded.Broadcast(EquippedRunes[SlotIndex]);
    OnEquippedRuneRemoved.Broadcast(SlotIndex, EquippedRunes[SlotIndex]);
    EquippedRunes[SlotIndex] = FMortisRuneInstance();       // 빈 슬롯으로 만들기

    OnActivatedRuneSetsChanged.Broadcast(GetActiveRuneSets());

    return true;
}

void UMortisRuneInventorySubsystem::ResetInventory()
{
    RuneSetMap.Empty();
    OwningRunes.Empty();
    EquippedRunes.Empty();
    ActivatedRuneSets.Empty();

    EquippedRunes.SetNum(SlotCount);
}

int32 UMortisRuneInventorySubsystem::GetSlotCount() const
{
    return SlotCount;
}

void UMortisRuneInventorySubsystem::SetSlotCount(const int32 NewCount)
{
    SlotCount = NewCount;
    EquippedRunes.SetNum(SlotCount);
}

void UMortisRuneInventorySubsystem::UpdateCooldown(const FGameplayTag& SetTag, float CooldownStartTime, float CooldownEndTime, int32 Level)
{
    FMortisActiveRuneSetState* FoundState = RuneSetMap.Find(SetTag);
    if (!FoundState) return;

    FoundState->Variables[Level-1].CooldownStartTime = CooldownStartTime;
    FoundState->Variables[Level-1].CooldownEndTime = CooldownEndTime;
    OnCoolTimeUpdated.Broadcast(SetTag, Level);
}

void UMortisRuneInventorySubsystem::UpdateDuration(const FGameplayTag& SetTag, float DurationStartTime, float DurationEndTime, int32 Level)
{
    FMortisActiveRuneSetState* FoundState = RuneSetMap.Find(SetTag);
    if (!FoundState) return;

    FoundState->Variables[Level-1].DurationStartTime = DurationStartTime;
    FoundState->Variables[Level-1].DurationEndTime = DurationEndTime;
    OnDurationTimeUpdated.Broadcast(SetTag, Level);
}

void UMortisRuneInventorySubsystem::UpdateStack(const FGameplayTag& SetTag, int32 Delta, int32 Level)
{
    FMortisActiveRuneSetState* FoundState = RuneSetMap.Find(SetTag);
    if (!FoundState) return;

    FoundState->Variables[Level-1].CurrentStack += Delta;
    OnStackUpdated.Broadcast(SetTag, Level);
}

const TArray<FMortisRuneInstance>& UMortisRuneInventorySubsystem::GetOwningRunes() const
{
    return OwningRunes;
}

const TArray<FMortisRuneInstance>& UMortisRuneInventorySubsystem::GetEquippedRunes() const
{
    return EquippedRunes;
}

const TArray<FGameplayTag>& UMortisRuneInventorySubsystem::GetActiveRuneSets() const
{
    return ActivatedRuneSets;
}

// 룬 세트효과 태그로 조회
bool UMortisRuneInventorySubsystem::GetRuneSetStateByTag(const FGameplayTag& TagToFind, FMortisActiveRuneSetState& OutState) const
{
    const FMortisActiveRuneSetState* FoundState = RuneSetMap.Find(TagToFind);
    if (!FoundState) return false;

    OutState = *FoundState;
    return true;
}

// 세트의 룬 장착 개수 변경 : Delta로 증감
void UMortisRuneInventorySubsystem::UpdateSetCount(const FGameplayTag& SetTagToChange, int32 Delta)
{
    if (!SetTagToChange.IsValid() || Delta == 0) return;
    UGameInstance* GI = GetGameInstance();
    check(GI);
    UMortisRuneDatabaseSubsystem* RuneDB = GI->GetSubsystem<UMortisRuneDatabaseSubsystem>();
    check(RuneDB);
    const FMortisRuneSetRow* SetRow = RuneDB->GetRuneSetRow(SetTagToChange);

    if (!RuneSetMap.Contains(SetTagToChange))
    {
        FMortisActiveRuneSetState& NewState = RuneSetMap.Add(SetTagToChange);
        NewState.SetTag = SetTagToChange;
        NewState.SetName = SetRow->SetName;
        
        for (const TObjectPtr<UTexture2D>& Tmp : SetRow->Icon)
        {
            NewState.Variables.Emplace(Tmp);
        }

        NewState.CurrentCount = 0;
        NewState.CurrentLevel = 0;
    }

    FMortisActiveRuneSetState* SetState = RuneSetMap.Find(SetTagToChange);
    check(SetState);
    SetState->CurrentCount += Delta;
    const int32 CalculatedLevel = GetLevelOfSet(SetRow, SetState->CurrentCount);
    if (SetState->CurrentLevel != CalculatedLevel)
    {
        SetState->CurrentLevel = CalculatedLevel;
        if (CalculatedLevel <= 0) ActivatedRuneSets.Remove(SetTagToChange);
        else ActivatedRuneSets.AddUnique(SetTagToChange);
    }
}

// Row의 세트효과에서 필요 Count 판별
const int32 UMortisRuneInventorySubsystem::GetLevelOfSet(const FMortisRuneSetRow* SetTagToCheck, const int32 CurrentCount) const
{
    int Level = 0;
    for (FMortisSetTierDef SetTier : SetTagToCheck->TierDefs)
    {
        if (SetTier.ActivateCount <= CurrentCount) Level++;
        else break;
    }

    return Level;
}
