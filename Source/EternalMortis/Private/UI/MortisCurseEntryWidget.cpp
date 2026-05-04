#include "UI/MortisCurseEntryWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"

void UMortisCurseEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_Root)
    {
        Button_Root->OnClicked.RemoveDynamic(this, &ThisClass::HandleRootButtonClicked);
        Button_Root->OnClicked.AddDynamic(this, &ThisClass::HandleRootButtonClicked);
    }

    RefreshVisual();
}

void UMortisCurseEntryWidget::NativeDestruct()
{
    if (Button_Root)
    {
        Button_Root->OnClicked.RemoveDynamic(this, &ThisClass::HandleRootButtonClicked);
    }

    Super::NativeDestruct();
}

void UMortisCurseEntryWidget::ApplyData(const FMortisCurseEntryViewData& InData)
{
    CurrentData = InData;
    RefreshVisual();
}

void UMortisCurseEntryWidget::SetSelected(bool bInSelected)
{
    CurrentData.bSelected = bInSelected;
    RefreshVisual();
}

void UMortisCurseEntryWidget::RefreshVisual()
{
    if (Image_CurseIcon)
    {
        Image_CurseIcon->SetBrushFromTexture(CurrentData.Icon, false);
        Image_CurseIcon->SetRenderOpacity(CurrentData.Icon ? 1.0f : 0.0f);
    }

    if (Text_CurseName)
    {
        Text_CurseName->SetText(CurrentData.DisplayName);
    }

    if (Border_ItemBg)
    {
        const FLinearColor BaseColor = CurrentData.bSelected
            ? FLinearColor(0.34f, 0.18f, 0.28f, 0.95f)
            : FLinearColor(0.10f, 0.08f, 0.12f, 0.82f);
        Border_ItemBg->SetBrushColor(BaseColor);
    }

    SetRenderOpacity(CurrentData.Curse.InstanceId.IsValid() ? 1.0f : 0.45f);
    ReceiveCurseEntryDataChanged(CurrentData);
}

void UMortisCurseEntryWidget::HandleRootButtonClicked()
{
    OnCurseEntryClicked.Broadcast(CurrentData.Curse);
}
