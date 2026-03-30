// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MortisRuneSynergySlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

namespace MortisRuneSynergySlotParams
{
    const FName RemainingRatio(TEXT("RemainingRatio"));
    const FName IconTex(TEXT("IconTex"));
    constexpr float UpdateInterval = 1.f / 60.f;
}

void FMortisTimedVisualState::SetWindow(float InStartTime, float InEndTime)
{
    StartTime = InStartTime;
    EndTime = InEndTime;
    TotalTime = FMath::Max(EndTime - StartTime, 0.f);
    RemainingTime = TotalTime;
    bTracking = TotalTime > KINDA_SMALL_NUMBER;
}

void FMortisTimedVisualState::Clear()
{
    StartTime = 0.f;
    EndTime = 0.f;
    TotalTime = 0.f;
    RemainingTime = 0.f;
    bTracking = false;
}

bool FMortisTimedVisualState::NeedsUpdate(float Now) const
{
    return bTracking && Now < EndTime;
}

float FMortisTimedVisualState::GetRemainingRatio(float Now) const
{
    if (TotalTime <= KINDA_SMALL_NUMBER)
    {
        return 0.f;
    }

    const float NewRemainingTime = FMath::Clamp(EndTime - Now, 0.f, TotalTime);
    return FMath::Clamp(NewRemainingTime / TotalTime, 0.f, 1.f);
}

void UMortisRuneSynergySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeDynamicMaterials();

    if (Image_CooldownMask)
    {
        Image_CooldownMask->SetVisibility(ESlateVisibility::Hidden);
    }

    if (Image_DurationFill)
    {
        Image_DurationFill->SetVisibility(ESlateVisibility::Hidden);
    }

    if (Text_StackCount)
    {
        Text_StackCount->SetVisibility(ESlateVisibility::Hidden);
    }

    RefreshVisualState();
}

void UMortisRuneSynergySlotWidget::NativeDestruct()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TimedVisualTimerHandle);
    }

    Super::NativeDestruct();
}

void UMortisRuneSynergySlotWidget::SetCurrentSetTag(FGameplayTag InSetTag)
{
    CurrentSetTag = InSetTag;
}

void UMortisRuneSynergySlotWidget::SetRuneIcon(UTexture2D* InIcon)
{
    CurrentIcon = InIcon;

    if (Image_Icon)
    {
        Image_Icon->SetBrushFromTexture(InIcon, false);
    }

    if (CooldownMID)
    {
        CooldownMID->SetTextureParameterValue(MortisRuneSynergySlotParams::IconTex, InIcon);
    }
}

void UMortisRuneSynergySlotWidget::SetStackCount(int32 InStackCount)
{
    CurrentStackCount = FMath::Max(0, InStackCount);

    if (!Text_StackCount)
    {
        return;
    }

    const bool bShowStack = CurrentStackCount > 0;
    Text_StackCount->SetVisibility(bShowStack ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);

    if (bShowStack)
    {
        Text_StackCount->SetText(FText::AsNumber(CurrentStackCount));
    }
}

void UMortisRuneSynergySlotWidget::SetCooldownWindow(float InStartTime, float InEndTime)
{
    CooldownState.SetWindow(InStartTime, InEndTime);
    RefreshVisualState();
}

void UMortisRuneSynergySlotWidget::ClearCooldown()
{
    CooldownState.Clear();

    if (CooldownMID)
    {
        CooldownMID->SetScalarParameterValue(MortisRuneSynergySlotParams::RemainingRatio, 0.f);
    }

    if (Image_CooldownMask)
    {
        Image_CooldownMask->SetVisibility(ESlateVisibility::Hidden);
    }

    UpdateTimedVisualTimer();
}

void UMortisRuneSynergySlotWidget::SetDurationWindow(float InStartTime, float InEndTime)
{
    DurationState.SetWindow(InStartTime, InEndTime);
    RefreshVisualState();
}

void UMortisRuneSynergySlotWidget::ClearDuration()
{
    DurationState.Clear();

    if (DurationMID)
    {
        DurationMID->SetScalarParameterValue(MortisRuneSynergySlotParams::RemainingRatio, 0.f);
    }

    if (Image_DurationFill)
    {
        Image_DurationFill->SetVisibility(ESlateVisibility::Hidden);
    }

    UpdateTimedVisualTimer();
}

void UMortisRuneSynergySlotWidget::RefreshVisualState()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const float Now = World->GetTimeSeconds();

    RefreshCooldownVisual(Now);
    RefreshDurationVisual(Now);
    UpdateTimedVisualTimer();
}

void UMortisRuneSynergySlotWidget::InitializeDynamicMaterials()
{
    if (Image_CooldownMask)
    {
        CooldownMID = Image_CooldownMask->GetDynamicMaterial();

        if (CooldownMID && CurrentIcon)
        {
            CooldownMID->SetTextureParameterValue(MortisRuneSynergySlotParams::IconTex, CurrentIcon);
        }
    }

    if (Image_DurationFill)
    {
        DurationMID = Image_DurationFill->GetDynamicMaterial();
    }
}

void UMortisRuneSynergySlotWidget::RefreshCooldownVisual(float Now)
{
    if (!Image_CooldownMask || !CooldownMID)
    {
        return;
    }

    if (!CooldownState.bTracking || CooldownState.TotalTime <= KINDA_SMALL_NUMBER)
    {
        CooldownMID->SetScalarParameterValue(MortisRuneSynergySlotParams::RemainingRatio, 0.f);
        Image_CooldownMask->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    if (Now < CooldownState.StartTime)
    {
        CooldownMID->SetScalarParameterValue(MortisRuneSynergySlotParams::RemainingRatio, 1.f);
        Image_CooldownMask->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    if (Now >= CooldownState.EndTime)
    {
        CooldownState.Clear();
        CooldownMID->SetScalarParameterValue(MortisRuneSynergySlotParams::RemainingRatio, 0.f);
        Image_CooldownMask->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    CooldownState.RemainingTime = FMath::Clamp(CooldownState.EndTime - Now, 0.f, CooldownState.TotalTime);

    const float RemainingRatio = CooldownState.GetRemainingRatio(Now);
    CooldownMID->SetScalarParameterValue(MortisRuneSynergySlotParams::RemainingRatio, RemainingRatio);
    Image_CooldownMask->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UMortisRuneSynergySlotWidget::RefreshDurationVisual(float Now)
{
    if (!Image_DurationFill || !DurationMID)
    {
        return;
    }

    if (!DurationState.bTracking || DurationState.TotalTime <= KINDA_SMALL_NUMBER)
    {
        DurationMID->SetScalarParameterValue(MortisRuneSynergySlotParams::RemainingRatio, 0.f);
        Image_DurationFill->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    if (Now < DurationState.StartTime)
    {
        DurationMID->SetScalarParameterValue(MortisRuneSynergySlotParams::RemainingRatio, 1.f);
        Image_DurationFill->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    if (Now >= DurationState.EndTime)
    {
        DurationState.Clear();
        DurationMID->SetScalarParameterValue(MortisRuneSynergySlotParams::RemainingRatio, 0.f);
        Image_DurationFill->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    DurationState.RemainingTime = FMath::Clamp(DurationState.EndTime - Now, 0.f, DurationState.TotalTime);

    const float RemainingRatio = DurationState.GetRemainingRatio(Now);
    DurationMID->SetScalarParameterValue(MortisRuneSynergySlotParams::RemainingRatio, RemainingRatio);
    Image_DurationFill->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UMortisRuneSynergySlotWidget::UpdateTimedVisualTimer()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const float Now = World->GetTimeSeconds();
    const bool bNeedsUpdate = CooldownState.NeedsUpdate(Now) || DurationState.NeedsUpdate(Now);

    FTimerManager& TimerManager = World->GetTimerManager();

    if (bNeedsUpdate)
    {
        if (!TimerManager.IsTimerActive(TimedVisualTimerHandle))
        {
            TimerManager.SetTimer(
                TimedVisualTimerHandle,
                this,
                &UMortisRuneSynergySlotWidget::HandleTimedVisualUpdate,
                MortisRuneSynergySlotParams::UpdateInterval,
                true);
        }
    }
    else
    {
        TimerManager.ClearTimer(TimedVisualTimerHandle);
    }
}

void UMortisRuneSynergySlotWidget::HandleTimedVisualUpdate()
{
    RefreshVisualState();
}
