#include "UI/MortisRuneSlotIconWidget.h"

#include "Components/Image.h"
#include "Styling/SlateBrush.h"

void UMortisRuneSlotIconWidget::InitializeSlot(int32 InSlotIndex, bool bInUnlocked)
{
	SlotIndex = InSlotIndex;
	bUnlocked = bInUnlocked;

	RefreshVisual();
}

void UMortisRuneSlotIconWidget::SetUnlocked(bool bInUnlocked)
{
	if (bUnlocked == bInUnlocked)
	{
		return;
	}

	bUnlocked = bInUnlocked;
	RefreshVisual();
}

void UMortisRuneSlotIconWidget::RefreshVisual()
{
	if (!IMG_RuneSlotIcon)
	{
		return;
	}

	UTexture2D* TargetIcon = bUnlocked ? UnlockedIcon : LockedIcon;
	if (TargetIcon)
	{
		IMG_RuneSlotIcon->SetBrushFromTexture(TargetIcon);
	}

	IMG_RuneSlotIcon->SetOpacity(bUnlocked ? UnlockedOpacity : LockedOpacity);
	IMG_RuneSlotIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
}
