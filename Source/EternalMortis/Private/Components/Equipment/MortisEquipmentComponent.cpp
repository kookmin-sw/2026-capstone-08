// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Equipment/MortisEquipmentComponent.h"
#include "Character/MortisCharacterBase.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "System/MortisRuneInventorySubsystem.h"
#include "System/MortisRuneDatabaseSubsystem.h"
#include "System/MortisCurseDatabaseSubsystem.h"
#include "System/MortisCurseInventorySubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayEffectTypes.h"

#include "MortisDebugHelper.h"

void UMortisEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();

    UGameInstance* GI = UGameplayStatics::GetGameInstance(GetWorld());
    check(GI);
    RuneDB = GI->GetSubsystem<UMortisRuneDatabaseSubsystem>();
    check(RuneDB);
    RuneInv = GI->GetSubsystem<UMortisRuneInventorySubsystem>();
    check(RuneInv);
    CurseDB = GI->GetSubsystem<UMortisCurseDatabaseSubsystem>();
    check(CurseDB);
    CurseInv = GI->GetSubsystem<UMortisCurseInventorySubsystem>();
    check(CurseInv);

    AMortisCharacterBase* MortisCharacter = GetOwningPawn<AMortisCharacterBase>();
    check(MortisCharacter);
    ASC = MortisCharacter->GetMortisAbilitySystemComponent();
    check(ASC);

    RuneInv->OnEquippedRuneAdded.AddDynamic(this, &UMortisEquipmentComponent::ApplyRuneEffect);
    RuneInv->OnEquippedRuneRemoved.AddDynamic(this, &UMortisEquipmentComponent::RemoveRuneEffect);
    RuneInv->OnActivatedRuneSetsChanged.AddDynamic(this, &UMortisEquipmentComponent::UpdateRuneSetBonus);

    CurseInv->OnCurseAdded.AddDynamic(this, &UMortisEquipmentComponent::ApplyCurseEffect);
    CurseInv->OnCurseRemoved.AddDynamic(this, &UMortisEquipmentComponent::RemoveCurseEffect);

    EquippedRuneRuntimes.SetNum(10);

    // 자동 장착
    for (const FMortisCurseInstance& Curse : CurseInv->GetOwnedCurses())
        ApplyCurseEffect(Curse);
}

void UMortisEquipmentComponent::ApplyRuneEffect(int32 SlotIndex, const FMortisRuneInstance& RuneToAdd)
{
    if (!ASC || !RuneDB || !EquippedRuneRuntimes.IsValidIndex(SlotIndex))
        return;

    FMortisEquippedRuneRuntime& EquippedRuneRuntime = EquippedRuneRuntimes[SlotIndex];

    // 이미 뭔가가 장착되어있다면 해제
    if (EquippedRuneRuntime.bActivated && EquippedRuneRuntime.EffectHandle.IsValid())
    {
        ASC->RemoveActiveGameplayEffect(EquippedRuneRuntime.EffectHandle);
        EquippedRuneRuntime = FMortisEquippedRuneRuntime{};
    }

    // 문양에 맞는 Effect 가져오기
    const FMortisRuneSymbolRow* SymbolRow = RuneDB->GetRuneSymbolRow(RuneToAdd.SymbolType);
    if (!SymbolRow || !SymbolRow->StatEffect) return;

    // Handle 생성
    const FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SymbolRow->StatEffect, 1.f, Context);
    if (!SpecHandle.IsValid()) return;

    // SetByCaller 설정
    SpecHandle.Data->SetSetByCallerMagnitude(MortisGameplayTags::Data_RuneValue, RuneToAdd.RolledValue);

    // ASC에 적용
    const FActiveGameplayEffectHandle ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    if (!ActiveHandle.WasSuccessfullyApplied()) return;

    // 내부 struct 정보 갱신
    EquippedRuneRuntime.bActivated = true;
    EquippedRuneRuntime.RuneInstanceId = RuneToAdd.InstanceId;
    EquippedRuneRuntime.Rune = RuneToAdd;
    EquippedRuneRuntime.EffectHandle = ActiveHandle;
}

void UMortisEquipmentComponent::RemoveRuneEffect(int32 SlotIndex, const FMortisRuneInstance& RuneToAdd)
{
    if (!ASC || !EquippedRuneRuntimes.IsValidIndex(SlotIndex))
        return;

    FMortisEquippedRuneRuntime& EquippedRuneRuntime = EquippedRuneRuntimes[SlotIndex];
    if (!EquippedRuneRuntime.bActivated) return;

    if (RuneToAdd.InstanceId.IsValid() &&
        EquippedRuneRuntime.RuneInstanceId.IsValid() &&
        RuneToAdd.InstanceId != EquippedRuneRuntime.RuneInstanceId)
            return;

    if (EquippedRuneRuntime.EffectHandle.IsValid())
        ASC->RemoveActiveGameplayEffect(EquippedRuneRuntime.EffectHandle);

    EquippedRuneRuntime = FMortisEquippedRuneRuntime{};
}

void UMortisEquipmentComponent::UpdateRuneSetBonus(const TArray<FGameplayTag>& ActiveSetTags)
{
    if (!RuneInv || !RuneDB || !ASC) return;

    TSet<FGameplayTag> IncomingTags;

    for (const FGameplayTag& SetTag : ActiveSetTags)
    {
        if (!SetTag.IsValid())
            continue;

        IncomingTags.Add(SetTag);

        FMortisActiveRuneSetState SetState;
        const bool bFound = RuneInv->GetRuneSetStateByTag(SetTag, SetState);

        if (!bFound || SetState.CurrentCount <= 0)
        {
            ClearSetRuntime(SetTag);
            continue;
        }

        const FMortisRuneSetRow* SetRow = RuneDB->GetRuneSetRow(SetTag);
        if (!SetRow)
        {
            ClearSetRuntime(SetTag);
            continue;
        }

        // 이 세트에서 지금 있어야 하는 티어들
        TSet<int32> DesiredTierCounts;
        for (const FMortisSetTierDef& TierDef : SetRow->TierDefs)
        {
            if (TierDef.ActivateCount <= SetState.CurrentCount)
                DesiredTierCounts.Add(TierDef.ActivateCount);
        }

        // 만족하는 티어가 하나도 없으면 제거
        if (DesiredTierCounts.Num() == 0)
        {
            ClearSetRuntime(SetTag);
            continue;
        }

        FMortisAppliedRuneSetRuntime& SetRuntime = AppliedRuneSetRuntimes.FindOrAdd(SetTag);
        SetRuntime.SetTag = SetTag;

        // 없어져야 할 티어만 제거
        for (int32 i = SetRuntime.AppliedTiers.Num() - 1; i >= 0; --i)
        {
            const int32 AppliedCount = SetRuntime.AppliedTiers[i].ActivateCount;

            if (!DesiredTierCounts.Contains(AppliedCount))
            {
                RemoveSetTier(SetRuntime, AppliedCount);
            }
        }

        // 새로 필요한 티어만 추가
        for (const FMortisSetTierDef& TierDef : SetRow->TierDefs)
        {
            if (!DesiredTierCounts.Contains(TierDef.ActivateCount))
                continue;

            if (HasAppliedTier(SetRuntime, TierDef.ActivateCount))
                continue;

            ApplySetTier(TierDef, SetRuntime);
        }

        // 적용된 티어가 아예 없으면 맵에서 제거
        if (SetRuntime.AppliedTiers.Num() == 0)
            AppliedRuneSetRuntimes.Remove(SetTag);
    }

    // 아예 사라진 세트 제거
    TArray<FGameplayTag> ExistingTags;
    AppliedRuneSetRuntimes.GetKeys(ExistingTags);

    for (const FGameplayTag& ExistingTag : ExistingTags)
    {
        if (!IncomingTags.Contains(ExistingTag))
            ClearSetRuntime(ExistingTag);
    }
}

void UMortisEquipmentComponent::ApplyCurseEffect(const FMortisCurseInstance& CurseToAdd)
{
    if (!ASC || !CurseDB || !CurseToAdd.CurseTag.IsValid())
        return;

    if (HasAppliedCurse(CurseToAdd.InstanceId))
        return;

    const FMortisCurseRow* CurseRow = CurseDB->GetCurseRow(CurseToAdd.CurseTag);
    if (!CurseRow || !CurseRow->CurseEffect)
        return;

    const FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CurseRow->CurseEffect, 1.f, Context);

    if (!SpecHandle.IsValid())
        return;

    SpecHandle.Data->SetSetByCallerMagnitude(MortisGameplayTags::Data_RuneValue, CurseToAdd.RolledValue);

    const FActiveGameplayEffectHandle Handle =
        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

    if (!Handle.WasSuccessfullyApplied())
        return;

    FMortisAppliedCurseRuntime Runtime;
    Runtime.CurseInstanceId = CurseToAdd.InstanceId;
    Runtime.Curse = CurseToAdd;
    Runtime.EffectHandle = Handle;

    AppliedCurseRuntimes.Add(Runtime);
}

void UMortisEquipmentComponent::RemoveCurseEffect(const FMortisCurseInstance& CurseToRemove)
{
    if (!ASC)
        return;

    for (int32 i = AppliedCurseRuntimes.Num() - 1; i >= 0; --i)
    {
        FMortisAppliedCurseRuntime& Runtime = AppliedCurseRuntimes[i];

        if (Runtime.CurseInstanceId != CurseToRemove.InstanceId)
            continue;

        if (Runtime.EffectHandle.IsValid())
            ASC->RemoveActiveGameplayEffect(Runtime.EffectHandle);

        AppliedCurseRuntimes.RemoveAt(i);
        return;
    }
}

bool UMortisEquipmentComponent::HasAppliedTier(const FMortisAppliedRuneSetRuntime& SetRuntime, int32 ActivateCount) const
{
    for (const FMortisAppliedRuneSetTierRuntime& TierRuntime : SetRuntime.AppliedTiers)
    {
        if (TierRuntime.ActivateCount == ActivateCount)
            return true;
    }

    return false;
}

bool UMortisEquipmentComponent::HasAppliedCurse(const FGuid& CurseInstanceId) const
{
    if (!CurseInstanceId.IsValid())
        return false;

    for (const FMortisAppliedCurseRuntime& Runtime : AppliedCurseRuntimes)
    {
        if (Runtime.CurseInstanceId == CurseInstanceId)
            return true;
    }

    return false;
}

void UMortisEquipmentComponent::ApplySetTier(const FMortisSetTierDef& TierDef, FMortisAppliedRuneSetRuntime& SetRuntime)
{
    if (!ASC) return;

    FMortisAppliedRuneSetTierRuntime NewTierRuntime;
    NewTierRuntime.ActivateCount = TierDef.ActivateCount;

    const FGameplayEffectContextHandle Context = ASC->MakeEffectContext();

    for (const TSubclassOf<UGameplayEffect>& EffectClass : TierDef.GrantedEffects)
    {
        if (!EffectClass)
            continue;

        FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.f, Context);
        if (!SpecHandle.IsValid())
            continue;

        const FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

        if (EffectHandle.WasSuccessfullyApplied())
            NewTierRuntime.EffectHandles.Add(EffectHandle);
    }

    for (const TSubclassOf<UGameplayAbility>& AbilityClass : TierDef.GrantedAbilities)
    {
        if (!AbilityClass)
            continue;

        FGameplayAbilitySpec AbilitySpec(AbilityClass, 1, INDEX_NONE, this);
        const FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(AbilitySpec);

        if (AbilityHandle.IsValid())
            NewTierRuntime.AbilityHandles.Add(AbilityHandle);
    }

    SetRuntime.AppliedTiers.Add(MoveTemp(NewTierRuntime));
}

void UMortisEquipmentComponent::RemoveSetTier(FMortisAppliedRuneSetRuntime& SetRuntime, int32 ActivateCount)
{
    if (!ASC) return;

    for (int32 i = SetRuntime.AppliedTiers.Num() - 1; i >= 0; i--)
    {
        FMortisAppliedRuneSetTierRuntime& TierRuntime = SetRuntime.AppliedTiers[i];
        if (TierRuntime.ActivateCount != ActivateCount)
            continue;

        for (const FActiveGameplayEffectHandle& EffectHandle : TierRuntime.EffectHandles)
        {
            if (EffectHandle.IsValid())
                ASC->RemoveActiveGameplayEffect(EffectHandle);
        }

        for (const FGameplayAbilitySpecHandle& AbilityHandle : TierRuntime.AbilityHandles)
        {
            if (AbilityHandle.IsValid())
                ASC->ClearAbility(AbilityHandle);
        }

        SetRuntime.AppliedTiers.RemoveAt(i);
        return;
    }
}

void UMortisEquipmentComponent::ClearSetRuntime(const FGameplayTag& SetTag)
{
    FMortisAppliedRuneSetRuntime* SetRuntime = AppliedRuneSetRuntimes.Find(SetTag);
    if (!SetRuntime)
        return;

    for (int32 i = SetRuntime->AppliedTiers.Num() - 1; i >= 0; i--)
    {
        RemoveSetTier(*SetRuntime, SetRuntime->AppliedTiers[i].ActivateCount);
    }

    AppliedRuneSetRuntimes.Remove(SetTag);
}

void UMortisEquipmentComponent::ClearCurseEffects()
{
    if (!ASC)
    {
        AppliedCurseRuntimes.Empty();
        return;
    }

    for (const FMortisAppliedCurseRuntime& Runtime : AppliedCurseRuntimes)
    {
        if (Runtime.EffectHandle.IsValid())
            ASC->RemoveActiveGameplayEffect(Runtime.EffectHandle);
    }

    AppliedCurseRuntimes.Empty();
}
