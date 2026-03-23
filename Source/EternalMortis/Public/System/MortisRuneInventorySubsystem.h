// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/MortisRuneDataTypes.h"
#include "GameplayTagContainer.h"
#include "MortisRuneInventorySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortisOnActivatedRuneSetsChanged, const TArray<FGameplayTag>&, ActiveRuneSetTags);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortisOnOwningRuneAdded, const FMortisRuneInstance&, RuneToAdd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortisOnOwningRuneRemoved, const FMortisRuneInstance&, RuneToRemove);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMortisOnEquippedRuneAdded, const int32, SlotIndex, const FMortisRuneInstance&, RuneToAdd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMortisOnEquippedRuneRemoved, const int32, SlotIndex, const FMortisRuneInstance&, RuneToRemove);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortisOnDurationTimeUpdated, const FGameplayTag&, TagToUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortisOnCoolTimeUpdated, const FGameplayTag&, TagToUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortisOnStackUpdated, const FGameplayTag&, TagToUpdate);


struct FMortisRuneSetRow;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisRuneInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    // UGameInstanceSubsystem Override
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // 혹시 몰라 룬 획득, 장착 및 해제, 세트 갱신마다 델리게이트를 등록해놓았으니, 필요하면 쓸 것
    UPROPERTY(BlueprintAssignable, Category = "Mortis|Rune")
    FMortisOnOwningRuneAdded OnOwningRuneAdded;
    UPROPERTY(BlueprintAssignable, Category = "Mortis|Rune")
    FMortisOnOwningRuneRemoved OnOwningRuneRemoved;
    UPROPERTY(BlueprintAssignable, Category = "Mortis|Rune")
    FMortisOnEquippedRuneAdded OnEquippedRuneAdded;
    UPROPERTY(BlueprintAssignable, Category = "Mortis|Rune")
    FMortisOnEquippedRuneRemoved OnEquippedRuneRemoved;
    UPROPERTY(BlueprintAssignable, Category = "Mortis|Rune")
    FMortisOnActivatedRuneSetsChanged OnActivatedRuneSetsChanged;
    UPROPERTY(BlueprintAssignable, Category = "Mortis|Rune")
    FMortisOnDurationTimeUpdated OnDurationTimeUpdated;
    UPROPERTY(BlueprintAssignable, Category = "Mortis|Rune")
    FMortisOnCoolTimeUpdated OnCoolTimeUpdated;
    UPROPERTY(BlueprintAssignable, Category = "Mortis|Rune")
    FMortisOnStackUpdated OnStackUpdated;

    // 룬 추가 | 장착 | 제거
    UFUNCTION(BlueprintCallable, Category = "Mortis|Rune")
    bool AddRune(const FMortisRuneInstance& NewRune);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Rune")
    bool EquipRune(int32 SlotIndex, const FMortisRuneInstance RuneInstance);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Rune")
    bool UnequipRune(int32 SlotIndex);

    // 초기화 용
    UFUNCTION(BlueprintCallable, Category = "Mortis|Rune")
    void ResetInventory();

    // 룬 장착 슬롯 설정
    UFUNCTION(BlueprintCallable, Category = "Mortis|Rune")
    int32 GetSlotCount() const;
    UFUNCTION(BlueprintCallable, Category = "Mortis|Rune")
    void SetSlotCount(const int32 NewCount);

    // 쿨타임, 지속시간, 스택 갱신
    UFUNCTION(BlueprintCallable, Category = "Mortis|Rune")
    void UpdateCooldown(const FGameplayTag& SetTag, float CooldownEndTime);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Rune")
    void UpdateDuration(const FGameplayTag& SetTag, float DurationEndTime);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Rune")
    void UpdateStack(const FGameplayTag& SetTag, int32 Delta);

    // Tag로 룬 세트 가져오기
    UFUNCTION(BlueprintPure, Category = "Mortis|Rune")
    bool GetRuneSetStateByTag(const FGameplayTag& TagToFind, FMortisActiveRuneSetState& OutState) const;

    // Getter들
    UFUNCTION(BlueprintPure, Category = "Mortis|Rune")
    const TArray<FMortisRuneInstance>& GetOwningRunes() const;

    UFUNCTION(BlueprintPure, Category = "Mortis|Rune")
    const TArray<FMortisRuneInstance>& GetEquippedRunes() const;

    UFUNCTION(BlueprintPure, Category = "Mortis|Rune")
    const TArray<FGameplayTag>& GetActiveRuneSets() const;

private:
    const int32 SetCount = 5;

    // 추후에 세이브파일 구현 시 따로 받아오도록 할 것
    UPROPERTY()
    int32 SlotCount = 6;

    // 캐싱용 룬 세트 Map
    UPROPERTY()
    TMap<FGameplayTag, FMortisActiveRuneSetState> RuneSetMap;

    // 소유, 장착, 활성화된 세트 배열
    UPROPERTY()
    TArray<FMortisRuneInstance> OwningRunes;

    UPROPERTY()
    TArray<FMortisRuneInstance> EquippedRunes;

    UPROPERTY()
    TArray<FGameplayTag> ActivatedRuneSets;

    // 내부용
    void UpdateSetCount(const FGameplayTag& SetTagToChange, int32 Delta);
    const int32 GetLevelOfSet(const FMortisRuneSetRow* SetTagToCheck, const int32 CurrentCount) const;
};
