#include "UI/MortisRightRuneDetailWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Styling/SlateColor.h"
#include "UI/MortisRuneSynergyEntryWidget.h"

namespace
{
void SetOptionalWidgetVisibility(UWidget* Widget, bool bVisible)
{
    if (Widget)
    {
        Widget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}
}

void UMortisRightRuneDetailWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_EquipRune)
    {
        Button_EquipRune->OnClicked.RemoveDynamic(this, &ThisClass::HandleEquipButtonClicked);
        Button_EquipRune->OnClicked.AddDynamic(this, &ThisClass::HandleEquipButtonClicked);
    }

    if (Button_UnequipRune)
    {
        Button_UnequipRune->OnClicked.RemoveDynamic(this, &ThisClass::HandleUnequipButtonClicked);
        Button_UnequipRune->OnClicked.AddDynamic(this, &ThisClass::HandleUnequipButtonClicked);
    }

    RefreshVisual();
}

void UMortisRightRuneDetailWidget::NativeDestruct()
{
    if (Button_EquipRune)
    {
        Button_EquipRune->OnClicked.RemoveDynamic(this, &ThisClass::HandleEquipButtonClicked);
    }

    if (Button_UnequipRune)
    {
        Button_UnequipRune->OnClicked.RemoveDynamic(this, &ThisClass::HandleUnequipButtonClicked);
    }

    Super::NativeDestruct();
}

void UMortisRightRuneDetailWidget::ApplyData(const FMortisRightRuneDetailViewData& InData)
{
    CurrentData = InData;
    RefreshVisual();
}

void UMortisRightRuneDetailWidget::RefreshVisual()
{
    const bool bHasRuneData = CurrentData.bHasRuneData;
    CurrentPresentationStyle = bHasRuneData ? CurrentData.RunePresentationStyle : FMortisRunePresentationStyle();

    SetOptionalWidgetVisibility(Border_RuneHeaderCard, bHasRuneData);
    SetOptionalWidgetVisibility(Spacer02, bHasRuneData);
    SetOptionalWidgetVisibility(Border_RuneSetSummaryCard, bHasRuneData);
    SetOptionalWidgetVisibility(Spacer03, bHasRuneData);
    SetOptionalWidgetVisibility(Border_RuneSynergyCard, bHasRuneData);
    SetOptionalWidgetVisibility(Spacer04, bHasRuneData);

    if (WidgetSwitcher_ActionState)
    {
        WidgetSwitcher_ActionState->SetVisibility(bHasRuneData ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    if (!bHasRuneData)
    {
        if (Text_SelectedRuneName)
        {
            Text_SelectedRuneName->SetText(FText::GetEmpty());
        }

        if (Text_RuneStatValue)
        {
            Text_RuneStatValue->SetText(FText::GetEmpty());
        }

        if (Text_SelectedSetName)
        {
            Text_SelectedSetName->SetText(FText::GetEmpty());
        }

        if (Text_SelectedSetProgress)
        {
            Text_SelectedSetProgress->SetText(FText::GetEmpty());
        }

        if (Text_RuneSynergyTitle)
        {
            Text_RuneSynergyTitle->SetText(FText::GetEmpty());
        }

        if (WidgetSwitcher_ActionState && WidgetSwitcher_ActionState->GetChildrenCount() > 0)
        {
            WidgetSwitcher_ActionState->SetActiveWidgetIndex(0);
        }

        if (Text_EquipRuneLabel)
        {
            Text_EquipRuneLabel->SetText(FText::GetEmpty());
        }

        if (Button_EquipRune)
        {
            Button_EquipRune->SetIsEnabled(false);
        }

        if (Text_UnequipRuneLabel)
        {
            Text_UnequipRuneLabel->SetText(FText::GetEmpty());
        }

        if (Text_UnequipCost)
        {
            Text_UnequipCost->SetText(FText::GetEmpty());
        }

        if (Text_CurrentGold)
        {
            Text_CurrentGold->SetText(FText::GetEmpty());
        }

        if (Button_UnequipRune)
        {
            Button_UnequipRune->SetIsEnabled(false);
        }

        ReceiveRunePresentationStyleChanged(CurrentPresentationStyle);

        if (Image_SelectedRuneCore)
        {
            Image_SelectedRuneCore->SetRenderOpacity(0.0f);
            Image_SelectedRuneCore->SetColorAndOpacity(FLinearColor::Transparent);
        }

        if (Image_SelectedRuneGlow)
        {
            Image_SelectedRuneGlow->SetBrushFromTexture(nullptr, false);
            Image_SelectedRuneGlow->SetRenderOpacity(0.0f);
            Image_SelectedRuneGlow->SetColorAndOpacity(FLinearColor::Transparent);
        }

        if (Image_SelectedRuneIcon)
        {
            Image_SelectedRuneIcon->SetBrushFromTexture(nullptr, false);
            Image_SelectedRuneIcon->SetRenderOpacity(0.0f);
            Image_SelectedRuneIcon->SetColorAndOpacity(FLinearColor::Transparent);
        }

        RefreshSynergyEntries();
        return;
    }

    if (Image_SelectedRuneCore)
    {
        Image_SelectedRuneCore->SetRenderOpacity(1.0f);
        Image_SelectedRuneCore->SetBrushTintColor(FSlateColor(CurrentPresentationStyle.CoreTint));
        Image_SelectedRuneCore->SetColorAndOpacity(CurrentPresentationStyle.CoreTint);
    }

    if (Image_SelectedRuneGlow)
    {
        Image_SelectedRuneGlow->SetBrushFromTexture(CurrentData.RuneIcon, false);
        Image_SelectedRuneGlow->SetBrushTintColor(FSlateColor(CurrentPresentationStyle.GlyphGlowTint));
        Image_SelectedRuneGlow->SetColorAndOpacity(FLinearColor::White);
        Image_SelectedRuneGlow->SetRenderOpacity(CurrentData.RuneIcon ? CurrentPresentationStyle.GlyphGlowOpacity : 0.0f);
    }

    if (Image_SelectedRuneIcon)
    {
        Image_SelectedRuneIcon->SetBrushFromTexture(CurrentData.RuneIcon, false);
        Image_SelectedRuneIcon->SetRenderOpacity(CurrentData.RuneIcon ? 1.0f : 0.0f);

        // Mirror the card-path tint behavior so the large detail glyph keeps the same
        // RuneSet color even if the widget image falls back to a white widget tint.
        Image_SelectedRuneIcon->SetBrushTintColor(FSlateColor(CurrentData.RuneIcon ? CurrentPresentationStyle.GlyphTint : FLinearColor::White));
        Image_SelectedRuneIcon->SetColorAndOpacity(FLinearColor::White);
    }

    if (Text_SelectedRuneName)
    {
        Text_SelectedRuneName->SetText(CurrentData.RuneName);
    }

    if (Text_RuneStatValue)
    {
        Text_RuneStatValue->SetText(CurrentData.RuneStatValue);
    }

    if (Text_SelectedSetName)
    {
        Text_SelectedSetName->SetText(CurrentData.SetName);
    }

    if (Text_SelectedSetProgress)
    {
        Text_SelectedSetProgress->SetText(CurrentData.SetProgress);
    }

    if (Text_RuneSynergyTitle)
    {
        Text_RuneSynergyTitle->SetText(NSLOCTEXT("MortisInventory", "RuneSynergyTitle", "세트 효과"));
    }

    if (WidgetSwitcher_ActionState)
    {
        int32 TargetActionIndex = 0;
        if (CurrentData.bShowUnequipAction)
        {
            TargetActionIndex = 2;
        }
        else if (CurrentData.bShowEquipAction)
        {
            TargetActionIndex = 1;
        }

        if (WidgetSwitcher_ActionState->GetChildrenCount() <= TargetActionIndex)
        {
            TargetActionIndex = 0;
        }

        WidgetSwitcher_ActionState->SetActiveWidgetIndex(TargetActionIndex);
    }

    if (Text_EquipRuneLabel)
    {
        Text_EquipRuneLabel->SetText(CurrentData.EquipLabel);
    }

    if (Button_EquipRune)
    {
        Button_EquipRune->SetIsEnabled(CurrentData.bCanEquip);
    }

    if (Text_UnequipRuneLabel)
    {
        Text_UnequipRuneLabel->SetText(CurrentData.UnequipLabel);
    }

    if (Text_UnequipCost)
    {
        Text_UnequipCost->SetText(CurrentData.UnequipCostText);
    }

    if (Text_CurrentGold)
    {
        Text_CurrentGold->SetText(CurrentData.CurrentGoldText);
    }

    if (Button_UnequipRune)
    {
        Button_UnequipRune->SetIsEnabled(CurrentData.bCanUnequip);
    }

    ReceiveRunePresentationStyleChanged(CurrentPresentationStyle);
    RefreshSynergyEntries();
}

void UMortisRightRuneDetailWidget::HandleEquipButtonClicked()
{
    OnEquipRequested.Broadcast();
}

void UMortisRightRuneDetailWidget::HandleUnequipButtonClicked()
{
    OnUnequipRequested.Broadcast();
}

void UMortisRightRuneDetailWidget::RefreshSynergyEntries()
{
    CreatedSynergyEntries.Reset();

    if (!VerticalBox_RuneSynergyListBody)
    {
        return;
    }

    VerticalBox_RuneSynergyListBody->ClearChildren();

    if (!RuneSynergyEntryClass)
    {
        return;
    }

    for (const FMortisRuneSynergyEntryViewData& EntryData : CurrentData.SynergyEntries)
    {
        UMortisRuneSynergyEntryWidget* EntryWidget = CreateWidget<UMortisRuneSynergyEntryWidget>(this, RuneSynergyEntryClass);
        if (!EntryWidget)
        {
            continue;
        }

        EntryWidget->ApplyData(EntryData);
        VerticalBox_RuneSynergyListBody->AddChild(EntryWidget);
        CreatedSynergyEntries.Add(EntryWidget);
    }
}
