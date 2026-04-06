#include "UI/MortisRuneSetEntryWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UMortisRuneSetEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_Root)
    {
        Button_Root->OnClicked.RemoveDynamic(this, &ThisClass::HandleRootButtonClicked);
        Button_Root->OnClicked.AddDynamic(this, &ThisClass::HandleRootButtonClicked);
    }

    RefreshVisual();
}

void UMortisRuneSetEntryWidget::ApplyData(FGameplayTag InSetTag, UTexture2D* InDisplayIcon, const FText& InDisplayName, int32 InOwnedCount, bool bInSelected)
{
    SetTag = InSetTag;
    DisplayIcon = InDisplayIcon;
    DisplayName = InDisplayName;
    OwnedCount = FMath::Max(0, InOwnedCount);
    bSelected = bInSelected;

    RefreshVisual();
}

void UMortisRuneSetEntryWidget::SetSelected(bool bInSelected)
{
    bSelected = bInSelected;
    RefreshVisual();
}

void UMortisRuneSetEntryWidget::RefreshVisual()
{
    if (Image_RuneSetIcon)
    {
        Image_RuneSetIcon->SetBrushFromTexture(DisplayIcon, false);
    }

    if (Text_RuneSetName)
    {
        Text_RuneSetName->SetText(DisplayName);
    }

    if (Text_RuneOwned)
    {
        Text_RuneOwned->SetText(FText::Format(
            NSLOCTEXT("MortisInventory", "RuneOwnedFormat", "Owned {0}"),
            FText::AsNumber(OwnedCount)));
    }

    if (Border_ItemBg)
    {
        const FLinearColor BaseColor = bSelected
            ? FLinearColor(0.22f, 0.22f, 0.30f, 1.0f)
            : FLinearColor(0.12f, 0.12f, 0.15f, 0.90f);
        Border_ItemBg->SetBrushColor(BaseColor);
    }

    SetRenderOpacity(OwnedCount > 0 ? 1.0f : 0.45f);
}

void UMortisRuneSetEntryWidget::HandleRootButtonClicked()
{
    OnSetEntryClicked.Broadcast(SetTag);
}
