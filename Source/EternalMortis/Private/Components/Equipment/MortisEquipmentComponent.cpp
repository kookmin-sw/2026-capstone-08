// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Equipment/MortisEquipmentComponent.h"
#include "Character/MortisCharacterBase.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "System/MortisRuneInventorySubsystem.h"
#include "System/MortisRuneDatabaseSubsystem.h"
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

    AMortisCharacterBase* MortisCharacter = GetOwningPawn<AMortisCharacterBase>();
    check(MortisCharacter);
    ASC = MortisCharacter->GetMortisAbilitySystemComponent();
    check(ASC);

    RuneInv->OnEquippedRuneAdded.AddDynamic(this, &UMortisEquipmentComponent::ApplyRuneEffect);
    RuneInv->OnEquippedRuneRemoved.AddDynamic(this, &UMortisEquipmentComponent::RemoveRuneEffect);
    RuneInv->OnActivatedRuneSetsChanged.AddDynamic(this, &UMortisEquipmentComponent::UpdateRuneSetBonus);

    EquippedRuneRuntimes.SetNum(10);
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

        // 세트 효과를 못찾았거나, 비활성화 되어있거나, 레벨이 0 이하라면 제거
        if (!bFound || SetState.CurrentLevel <= 0)
        {
            RemoveSetTier(SetTag);
            continue;
        }

        // 없는 세트효과면 제거
        const FMortisRuneSetRow* SetRow = RuneDB->GetRuneSetRow(SetTag);
        if (!SetRow)
        {
            RemoveSetTier(SetTag);
            continue;
        }

        // 적용할 Ability/Effect를 못찾으면 제거
        const FMortisSetTierDef* TierDef = FindTierDefByLevel(*SetRow, SetState.CurrentLevel);
        if (!TierDef)
        {
            RemoveSetTier(SetTag);
            continue;
        }

        FMortisAppliedRuneSetRuntime* ExistingRuntime = AppliedRuneSetRuntimes.Find(SetTag);
        // 이미 같은 등급이면 넘어가기
        if (ExistingRuntime && ExistingRuntime->AppliedLevel == SetState.CurrentLevel)
            continue;

        if (ExistingRuntime)
            RemoveSetTier(SetTag);

        // 모두 정상에, 갱신이 필요하면 세트효과 적용
        ApplySetTier(SetState, *TierDef);
    }

    // 없어진 세트효과 제거
    TArray<FGameplayTag> ExistingTags;
    AppliedRuneSetRuntimes.GetKeys(ExistingTags);

    for (const FGameplayTag& ExistingTag : ExistingTags)
    {
        if (!IncomingTags.Contains(ExistingTag))
            RemoveSetTier(ExistingTag);
    }
}

// 도우미 함수
const FMortisSetTierDef* UMortisEquipmentComponent::FindTierDefByLevel(const FMortisRuneSetRow& SetRow, int32 Level) const
{
    if (Level <= 0)return nullptr;

    const int32 Index = Level - 1;
    if (!SetRow.TierDefs.IsValidIndex(Index)) return nullptr;

    return &SetRow.TierDefs[Index];
}

void UMortisEquipmentComponent::ApplySetTier(const FMortisActiveRuneSetState& SetState, const FMortisSetTierDef& TierDef)
{
    MORTIS_LOG("Hello");
    if (!ASC) return;

    FMortisAppliedRuneSetRuntime RuneSetRuntime;
    RuneSetRuntime.SetTag = SetState.SetTag;
    RuneSetRuntime.AppliedLevel = SetState.CurrentLevel;

    const FGameplayEffectContextHandle Context = ASC->MakeEffectContext();

    for (const TSubclassOf<UGameplayEffect>& EffectClass : TierDef.GrantedEffects)
    {
        if (!EffectClass)
            continue;

        FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.f, Context);
        if (!SpecHandle.IsValid())
            continue;

        const FActiveGameplayEffectHandle EffectHandle =
            ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

        if (EffectHandle.WasSuccessfullyApplied())
            RuneSetRuntime.EffectHandles.Add(EffectHandle);
    }

    if (GetOwningPawn())
    {
        for (const TSubclassOf<UGameplayAbility>& AbilityClass : TierDef.GrantedAbilities)
        {
            if (!AbilityClass)
                continue;

            FGameplayAbilitySpec AbilitySpec(AbilityClass, 1, INDEX_NONE, this);
            const FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(AbilitySpec);

            if (AbilityHandle.IsValid())
                RuneSetRuntime.AbilityHandles.Add(AbilityHandle);
        }
    }

    AppliedRuneSetRuntimes.Add(SetState.SetTag, MoveTemp(RuneSetRuntime));
}

void UMortisEquipmentComponent::RemoveSetTier(const FGameplayTag& SetTag)
{
    if (!ASC) return;

    FMortisAppliedRuneSetRuntime* Runtime = AppliedRuneSetRuntimes.Find(SetTag);
    if (!Runtime) return;

    for (const FActiveGameplayEffectHandle& EffectHandle : Runtime->EffectHandles)
    {
        if (EffectHandle.IsValid())
            ASC->RemoveActiveGameplayEffect(EffectHandle);
    }

    if (GetOwner() && GetOwner()->HasAuthority())
    {
        for (const FGameplayAbilitySpecHandle& AbilityHandle : Runtime->AbilityHandles)
            if (AbilityHandle.IsValid())
                ASC->ClearAbility(AbilityHandle);
    }

    AppliedRuneSetRuntimes.Remove(SetTag);
}
