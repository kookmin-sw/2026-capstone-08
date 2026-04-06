#include "UI/MortisRightRuneDetailWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "UI/MortisRuneSynergyEntryWidget.h"

void UMortisRightRuneDetailWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_EquipRune)
    {
        Button_EquipRune->OnClicked.RemoveDynamic(this, &ThisClass::HandleEquipButtonClicked);
        Button_EquipRune->OnClicked.AddDynamic(this, &ThisClass::HandleEquipButtonClicked);
    }

    RefreshVisual();
}

void UMortisRightRuneDetailWidget::NativeDestruct()
{
    if (Button_EquipRune)
    {
        Button_EquipRune->OnClicked.RemoveDynamic(this, &ThisClass::HandleEquipButtonClicked);
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
    if (Image_SelectedRuneIcon)
    {
        Image_SelectedRuneIcon->SetBrushFromTexture(CurrentData.RuneIcon, false);
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
        WidgetSwitcher_ActionState->SetActiveWidgetIndex(CurrentData.bShowEquipAction ? 1 : 0);
    }

    if (Text_EquipRuneLabel)
    {
        Text_EquipRuneLabel->SetText(CurrentData.EquipLabel);
    }

    if (Button_EquipRune)
    {
        Button_EquipRune->SetIsEnabled(CurrentData.bCanEquip);
    }

    RefreshSynergyEntries();
}

void UMortisRightRuneDetailWidget::HandleEquipButtonClicked()
{
    OnEquipRequested.Broadcast();
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
