#include "UI/MortisRuneSlotUnlockPanelWidget.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Engine/GameInstance.h"
#include "System/MortisMetaProgressionSubsystem.h"
#include "UI/MortisRuneSlotIconWidget.h"
#include "UObject/SoftObjectPath.h"

void UMortisRuneSlotUnlockPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeReferences();
	BindWidgetEvents();
	BindMetaDelegates();

	if (TXT_RuneSlotTitle)
	{
		TXT_RuneSlotTitle->SetText(RuneSlotTitleText);
	}

	if (TXT_UnlockRuneSlot)
	{
		TXT_UnlockRuneSlot->SetText(UnlockRuneSlotText);
	}

	RefreshAll();
}

void UMortisRuneSlotUnlockPanelWidget::NativeDestruct()
{
	UnbindWidgetEvents();
	UnbindMetaDelegates();

	Super::NativeDestruct();
}

void UMortisRuneSlotUnlockPanelWidget::RefreshAll()
{
	InitializeReferences();
	RebuildSlotIcons();
	RefreshSlotIconStates();
	RefreshTexts();
	RefreshUnlockButton();
}

void UMortisRuneSlotUnlockPanelWidget::RefreshFocus()
{
	if (BTN_UnlockRuneSlot && BTN_UnlockRuneSlot->GetIsEnabled())
	{
		BTN_UnlockRuneSlot->SetKeyboardFocus();
	}
}

void UMortisRuneSlotUnlockPanelWidget::HandleUnlockRuneSlotClicked()
{
	if (!MetaSubsystemRef)
	{
		return;
	}

	if (MetaSubsystemRef->UpgradeRuneSlot())
	{
		RefreshAll();
	}
	else
	{
		RefreshTexts();
		RefreshUnlockButton();
	}
}

void UMortisRuneSlotUnlockPanelWidget::HandleMemoryFragmentsChanged(int32 NewValue)
{
	RefreshTexts();
	RefreshUnlockButton();
}

void UMortisRuneSlotUnlockPanelWidget::InitializeReferences()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MetaSubsystemRef = GameInstance->GetSubsystem<UMortisMetaProgressionSubsystem>();
	}
}

void UMortisRuneSlotUnlockPanelWidget::BindWidgetEvents()
{
	if (BTN_UnlockRuneSlot)
	{
		BTN_UnlockRuneSlot->OnClicked.RemoveDynamic(this, &ThisClass::HandleUnlockRuneSlotClicked);
		BTN_UnlockRuneSlot->OnClicked.AddDynamic(this, &ThisClass::HandleUnlockRuneSlotClicked);
	}
}

void UMortisRuneSlotUnlockPanelWidget::UnbindWidgetEvents()
{
	if (BTN_UnlockRuneSlot)
	{
		BTN_UnlockRuneSlot->OnClicked.RemoveDynamic(this, &ThisClass::HandleUnlockRuneSlotClicked);
	}
}

void UMortisRuneSlotUnlockPanelWidget::BindMetaDelegates()
{
	if (!MetaSubsystemRef)
	{
		return;
	}

	MetaSubsystemRef->OnMemoryFragmentsChanged.RemoveDynamic(this, &ThisClass::HandleMemoryFragmentsChanged);
	MetaSubsystemRef->OnMemoryFragmentsChanged.AddDynamic(this, &ThisClass::HandleMemoryFragmentsChanged);
}

void UMortisRuneSlotUnlockPanelWidget::UnbindMetaDelegates()
{
	if (MetaSubsystemRef)
	{
		MetaSubsystemRef->OnMemoryFragmentsChanged.RemoveDynamic(this, &ThisClass::HandleMemoryFragmentsChanged);
	}
}

void UMortisRuneSlotUnlockPanelWidget::RebuildSlotIcons()
{
	if (!HB_RuneSlotIcons)
	{
		return;
	}

	HB_RuneSlotIcons->ClearChildren();
	CreatedSlotIcons.Reset();

	const TSubclassOf<UMortisRuneSlotIconWidget> ResolvedIconClass = ResolveRuneSlotIconClass();
	if (!MetaSubsystemRef || !ResolvedIconClass)
	{
		return;
	}

	const int32 MaxSlotCount = MetaSubsystemRef->GetMaxRuneSlotCount();
	const int32 UnlockedSlotCount = MetaSubsystemRef->GetUnlockedRuneSlotCount();

	for (int32 SlotIndex = 0; SlotIndex < MaxSlotCount; ++SlotIndex)
	{
		UMortisRuneSlotIconWidget* SlotIcon = CreateWidget<UMortisRuneSlotIconWidget>(this, ResolvedIconClass);
		if (!SlotIcon)
		{
			continue;
		}

		SlotIcon->InitializeSlot(SlotIndex, SlotIndex < UnlockedSlotCount);
		HB_RuneSlotIcons->AddChild(SlotIcon);
		CreatedSlotIcons.Add(SlotIcon);
	}
}

TSubclassOf<UMortisRuneSlotIconWidget> UMortisRuneSlotUnlockPanelWidget::ResolveRuneSlotIconClass() const
{
	if (RuneSlotIconClass)
	{
		return RuneSlotIconClass;
	}

	static const FSoftClassPath DefaultRuneSlotIconClassPath(TEXT("/Game/UI/MainMenu/WBP_RuneSlotIconWidget.WBP_RuneSlotIconWidget_C"));
	return DefaultRuneSlotIconClassPath.TryLoadClass<UMortisRuneSlotIconWidget>();
}

void UMortisRuneSlotUnlockPanelWidget::RefreshSlotIconStates()
{
	if (!MetaSubsystemRef)
	{
		return;
	}

	const int32 UnlockedSlotCount = MetaSubsystemRef->GetUnlockedRuneSlotCount();
	for (int32 SlotIndex = 0; SlotIndex < CreatedSlotIcons.Num(); ++SlotIndex)
	{
		if (CreatedSlotIcons[SlotIndex])
		{
			CreatedSlotIcons[SlotIndex]->SetUnlocked(SlotIndex < UnlockedSlotCount);
		}
	}
}

void UMortisRuneSlotUnlockPanelWidget::RefreshTexts()
{
	const int32 UnlockedSlotCount = MetaSubsystemRef ? MetaSubsystemRef->GetUnlockedRuneSlotCount() : 0;
	const int32 MaxSlotCount = MetaSubsystemRef ? MetaSubsystemRef->GetMaxRuneSlotCount() : 0;

	if (TXT_RuneSlotTitle)
	{
		if (TXT_RuneSlotCount || !MetaSubsystemRef)
		{
			TXT_RuneSlotTitle->SetText(RuneSlotTitleText);
		}
		else
		{
			TXT_RuneSlotTitle->SetText(FText::Format(
				RuneSlotTitleWithCountFormatText,
				RuneSlotTitleText,
				FText::AsNumber(UnlockedSlotCount),
				FText::AsNumber(MaxSlotCount)
			));
		}
	}

	if (TXT_RuneSlotCount)
	{
		if (MetaSubsystemRef)
		{
			TXT_RuneSlotCount->SetText(FText::Format(
				RuneSlotCountFormatText,
				FText::AsNumber(UnlockedSlotCount),
				FText::AsNumber(MaxSlotCount)
			));
		}
		else
		{
			TXT_RuneSlotCount->SetText(FText::GetEmpty());
		}
	}

	if (TXT_UnlockRuneSlot)
	{
		TXT_UnlockRuneSlot->SetText(UnlockRuneSlotText);
	}

	if (!TXT_RuneSlotCost)
	{
		return;
	}

	if (!MetaSubsystemRef)
	{
		TXT_RuneSlotCost->SetText(FText::GetEmpty());
		return;
	}

	int32 Cost = 0;
	if (!GetNextUpgradeCost(Cost))
	{
		TXT_RuneSlotCost->SetText(MaxRuneSlotText);
		return;
	}

	TXT_RuneSlotCost->SetText(FText::Format(
		NSLOCTEXT("MortisMetaProgression", "RuneSlotCostFormat", "Cost: {0} / {1}"),
		FText::AsNumber(Cost),
		FText::AsNumber(GetMemoryFragments())
	));
}

void UMortisRuneSlotUnlockPanelWidget::RefreshUnlockButton()
{
	if (!BTN_UnlockRuneSlot)
	{
		return;
	}

	BTN_UnlockRuneSlot->SetIsEnabled(CanAttemptRuneSlotUpgrade());
}

int32 UMortisRuneSlotUnlockPanelWidget::GetMemoryFragments() const
{
	return MetaSubsystemRef ? MetaSubsystemRef->GetMemoryFragments() : 0;
}

bool UMortisRuneSlotUnlockPanelWidget::GetNextUpgradeCost(int32& OutCost) const
{
	OutCost = 0;
	return MetaSubsystemRef && MetaSubsystemRef->GetNextRuneSlotUpgradeCost(OutCost);
}

bool UMortisRuneSlotUnlockPanelWidget::IsAtMaxRuneSlots() const
{
	return MetaSubsystemRef && MetaSubsystemRef->GetUnlockedRuneSlotCount() >= MetaSubsystemRef->GetMaxRuneSlotCount();
}

bool UMortisRuneSlotUnlockPanelWidget::CanAttemptRuneSlotUpgrade() const
{
	int32 Cost = 0;
	return MetaSubsystemRef && GetNextUpgradeCost(Cost) && GetMemoryFragments() >= Cost;
}
