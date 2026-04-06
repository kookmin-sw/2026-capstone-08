#include "UI/MortisRuneSynergyEntryWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UMortisRuneSynergyEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();
    RefreshVisual();
}

void UMortisRuneSynergyEntryWidget::ApplyData(const FMortisRuneSynergyEntryViewData& InData)
{
    CurrentData = InData;
    RefreshVisual();
}

void UMortisRuneSynergyEntryWidget::SetActiveState(bool bInActive)
{
    CurrentData.bIsActive = bInActive;
    RefreshVisual();
}

void UMortisRuneSynergyEntryWidget::RefreshVisual()
{
    if (Image_SynergyIcon)
    {
        Image_SynergyIcon->SetBrushFromTexture(CurrentData.DisplayIcon, false);
    }

    if (Text_TierCount)
    {
        Text_TierCount->SetText(FText::AsNumber(CurrentData.RequiredCount));
    }

    if (Text_SynergyTitle)
    {
        Text_SynergyTitle->SetText(CurrentData.TitleText);
    }

    if (Text_SynergyDescription)
    {
        Text_SynergyDescription->SetText(CurrentData.DescriptionText);
    }

    if (Border_EntryBg)
    {
        Border_EntryBg->SetBrushColor(
            CurrentData.bIsActive
                ? FLinearColor(0.18f, 0.20f, 0.24f, 1.0f)
                : FLinearColor(0.10f, 0.10f, 0.12f, 0.80f));
    }

    SetRenderOpacity(CurrentData.bIsActive ? 1.0f : 0.65f);
}
