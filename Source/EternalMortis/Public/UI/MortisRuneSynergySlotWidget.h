// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/MortisWidgetBase.h"
#include "MortisRuneSynergySlotWidget.generated.h"

class UImage;
class UMaterialInstanceDynamic;
class UTextBlock;
class UTexture2D;

USTRUCT(BlueprintType)
struct FMortisTimedVisualState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RuneSynergy")
    float StartTime = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RuneSynergy")
    float EndTime = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RuneSynergy")
    float TotalTime = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RuneSynergy")
    float RemainingTime = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|RuneSynergy")
    bool bTracking = false;

    void SetWindow(float InStartTime, float InEndTime);
    void Clear();
    bool NeedsUpdate(float Now) const;
    float GetRemainingRatio(float Now) const;
};

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisRuneSynergySlotWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mortis|RuneSynergy")
    void SetCurrentSetTag(FGameplayTag InSetTag);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RuneSynergy")
    void SetRuneIcon(UTexture2D* InIcon);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RuneSynergy")
    void SetStackCount(int32 InStackCount);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RuneSynergy")
    void SetCooldownWindow(float InStartTime, float InEndTime);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RuneSynergy")
    void ClearCooldown();

    UFUNCTION(BlueprintCallable, Category = "Mortis|RuneSynergy")
    void SetDurationWindow(float InStartTime, float InEndTime);

    UFUNCTION(BlueprintCallable, Category = "Mortis|RuneSynergy")
    void ClearDuration();

    UFUNCTION(BlueprintCallable, Category = "Mortis|RuneSynergy")
    void RefreshVisualState();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_Icon = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_CooldownMask = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_DurationFill = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_StackCount = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|RuneSynergy", meta = (AllowPrivateAccess = "true"))
    FGameplayTag CurrentSetTag;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|RuneSynergy", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> CurrentIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|RuneSynergy", meta = (AllowPrivateAccess = "true"))
    int32 CurrentStackCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|RuneSynergy", meta = (AllowPrivateAccess = "true"))
    FMortisTimedVisualState CooldownState;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|RuneSynergy", meta = (AllowPrivateAccess = "true"))
    FMortisTimedVisualState DurationState;

    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> CooldownMID = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> DurationMID = nullptr;

private:
    void InitializeDynamicMaterials();
    void RefreshCooldownVisual(float Now);
    void RefreshDurationVisual(float Now);
    void UpdateTimedVisualTimer();
    void HandleTimedVisualUpdate();

    FTimerHandle TimedVisualTimerHandle;
};
