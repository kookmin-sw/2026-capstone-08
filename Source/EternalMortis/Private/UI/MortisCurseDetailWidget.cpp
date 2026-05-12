#include "UI/MortisCurseDetailWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UMortisCurseDetailWidget::ApplyData(const FMortisCurseDetailViewData& InData)
{
    CurrentData = InData;
    RefreshVisual();
}

void UMortisCurseDetailWidget::ClearData()
{
    CurrentData = FMortisCurseDetailViewData();
    RefreshVisual();
}

void UMortisCurseDetailWidget::RefreshVisual()
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

    if (Text_CurseDescription)
    {
        Text_CurseDescription->SetText(CurrentData.Description);
    }

    ReceiveCurseDetailDataChanged(CurrentData);
}
