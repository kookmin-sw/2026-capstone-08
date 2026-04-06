#include "UI/MortisRuneCardWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UMortisRuneCardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_Root)
    {
        Button_Root->OnClicked.RemoveDynamic(this, &ThisClass::HandleRootButtonClicked);
        Button_Root->OnClicked.AddDynamic(this, &ThisClass::HandleRootButtonClicked);
    }

    RefreshVisual();
}

void UMortisRuneCardWidget::ApplyData(const FMortisRuneInstance& InRuneInstance, UTexture2D* InDisplayIcon, bool bInSelected, bool bInEquipped)
{
    RuneInstance = InRuneInstance;
    DisplayIcon = InDisplayIcon;
    bSelected = bInSelected;
    bEquipped = bInEquipped;

    RefreshVisual();
}

void UMortisRuneCardWidget::SetSelected(bool bInSelected)
{
    bSelected = bInSelected;
    RefreshVisual();
}

void UMortisRuneCardWidget::SetEquipped(bool bInEquipped)
{
    bEquipped = bInEquipped;
    RefreshVisual();
}

void UMortisRuneCardWidget::RefreshVisual()
{
    if (Image_RuneIcon)
    {
        Image_RuneIcon->SetBrushFromTexture(DisplayIcon, false);
    }

    if (Border_CardBg)
    {
        FLinearColor BaseColor = bSelected
            ? FLinearColor(0.22f, 0.24f, 0.30f, 1.0f)
            : FLinearColor(0.11f, 0.11f, 0.14f, 0.95f);

        if (bEquipped && !bSelected)
        {
            BaseColor = FLinearColor(0.14f, 0.16f, 0.18f, 0.95f);
        }

        Border_CardBg->SetBrushColor(BaseColor);
    }
}

void UMortisRuneCardWidget::HandleRootButtonClicked()
{
    OnRuneCardClicked.Broadcast(RuneInstance);
}
