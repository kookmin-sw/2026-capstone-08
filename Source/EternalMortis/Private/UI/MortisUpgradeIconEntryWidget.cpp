#include "UI/MortisUpgradeIconEntryWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Styling/SlateBrush.h"

void UMortisUpgradeIconEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BTN_Icon)
	{
		BTN_Icon->OnClicked.RemoveDynamic(this, &ThisClass::HandleIconClicked);
		BTN_Icon->OnClicked.AddDynamic(this, &ThisClass::HandleIconClicked);
	}

	RefreshVisual();
}

void UMortisUpgradeIconEntryWidget::InitializeEntry(const FMortisExperienceRow& InExperienceRow, bool bInUnlocked, bool bInActive)
{
	InitializeManualEntry(InExperienceRow.ExperienceTag, InExperienceRow.DisplayName, InExperienceRow.Icon, bInUnlocked, bInActive, false);
}

void UMortisUpgradeIconEntryWidget::InitializeManualEntry(FGameplayTag InExperienceTag, const FText& InDisplayName, UTexture2D* InIcon, bool bInUnlocked, bool bInActive, bool bInNoUpgradeEntry)
{
	ExperienceTag = InExperienceTag;
	DisplayName = InDisplayName;
	Icon = InIcon;
	bUnlocked = bInUnlocked;
	bActive = bInActive;
	bNoUpgradeEntry = bInNoUpgradeEntry;

	RefreshVisual();
}

void UMortisUpgradeIconEntryWidget::SetActive(bool bInActive)
{
	if (bActive == bInActive)
	{
		return;
	}

	bActive = bInActive;
	RefreshVisual();
}

void UMortisUpgradeIconEntryWidget::SetUnlocked(bool bInUnlocked)
{
	if (bUnlocked == bInUnlocked)
	{
		return;
	}

	bUnlocked = bInUnlocked;
	RefreshVisual();
}

void UMortisUpgradeIconEntryWidget::RefreshVisual()
{
	if (TXT_Name)
	{
		TXT_Name->SetText(DisplayName);
	}

	if (IMG_Icon)
	{
		if (Icon)
		{
			IMG_Icon->SetBrushFromTexture(Icon);
			IMG_Icon->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			IMG_Icon->SetBrush(FSlateBrush());
			IMG_Icon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	const ESlateVisibility LockVisibility = bUnlocked
		? ESlateVisibility::Collapsed
		: ESlateVisibility::HitTestInvisible;

	if (BD_LockDim)
	{
		BD_LockDim->SetVisibility(LockVisibility);
	}

	if (IMG_Lock)
	{
		IMG_Lock->SetVisibility(LockVisibility);
	}

	if (IMG_ActiveRing)
	{
		IMG_ActiveRing->SetVisibility(bActive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UMortisUpgradeIconEntryWidget::HandleIconClicked()
{
	OnUpgradeEntryClicked.Broadcast(ExperienceTag, bNoUpgradeEntry);
}
