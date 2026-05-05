#include "UI/MortisUpgradeSelectWidget.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Styling/SlateBrush.h"
#include "System/MortisGameDataSettings.h"
#include "System/MortisMetaProgressionSubsystem.h"
#include "UI/MortisUpgradeIconEntryWidget.h"

void UMortisUpgradeSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeReferences();
	BindWidgetEvents();
	BindMetaDelegates();

	if (TXT_CostLabel)
	{
		TXT_CostLabel->SetText(CostLabelText);
	}

	RefreshData();
}

void UMortisUpgradeSelectWidget::NativeDestruct()
{
	UnbindWidgetEvents();
	UnbindMetaDelegates();

	Super::NativeDestruct();
}

void UMortisUpgradeSelectWidget::RefreshData()
{
	InitializeReferences();
	RebuildEntryList();
	SelectDefaultEntry();
	RefreshEntryStates();
	RefreshDetailPanel();
	RefreshActionButton();
}

void UMortisUpgradeSelectWidget::RefreshFocus()
{
	if (BTN_Action && BTN_Action->GetIsEnabled())
	{
		BTN_Action->SetKeyboardFocus();
	}
}

bool UMortisUpgradeSelectWidget::GetActiveEntryData(FMortisUpgradeSelectEntryData& OutEntryData) const
{
	if (!bHasActiveEntry)
	{
		return false;
	}

	OutEntryData = ActiveEntryData;
	return true;
}

void UMortisUpgradeSelectWidget::HandleEntryClicked(FGameplayTag ClickedExperienceTag, bool bNoUpgradeEntry)
{
	SetActiveEntry(ClickedExperienceTag, bNoUpgradeEntry);
}

void UMortisUpgradeSelectWidget::HandleActionClicked()
{
	if (!bHasActiveEntry)
	{
		return;
	}

	if (IsEntryUnlocked(ActiveEntryData))
	{
		StartRunWithActiveEntry();
		return;
	}

	TryUnlockActiveEntry();
}

void UMortisUpgradeSelectWidget::HandleMemoryFragmentsChanged(int32 NewValue)
{
	RefreshDetailPanel();
	RefreshActionButton();
}

void UMortisUpgradeSelectWidget::InitializeReferences()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MetaSubsystemRef = GameInstance->GetSubsystem<UMortisMetaProgressionSubsystem>();
	}

	const UMortisGameDataSettings* Settings = GetDefault<UMortisGameDataSettings>();
	if (Settings)
	{
		ExperienceTableRef = Settings->ExperienceTable.LoadSynchronous();
	}
}

void UMortisUpgradeSelectWidget::BindWidgetEvents()
{
	if (BTN_Action)
	{
		BTN_Action->OnClicked.RemoveDynamic(this, &ThisClass::HandleActionClicked);
		BTN_Action->OnClicked.AddDynamic(this, &ThisClass::HandleActionClicked);
	}
}

void UMortisUpgradeSelectWidget::UnbindWidgetEvents()
{
	if (BTN_Action)
	{
		BTN_Action->OnClicked.RemoveDynamic(this, &ThisClass::HandleActionClicked);
	}
}

void UMortisUpgradeSelectWidget::BindMetaDelegates()
{
	if (!MetaSubsystemRef)
	{
		return;
	}

	MetaSubsystemRef->OnMemoryFragmentsChanged.RemoveDynamic(this, &ThisClass::HandleMemoryFragmentsChanged);
	MetaSubsystemRef->OnMemoryFragmentsChanged.AddDynamic(this, &ThisClass::HandleMemoryFragmentsChanged);
}

void UMortisUpgradeSelectWidget::UnbindMetaDelegates()
{
	if (MetaSubsystemRef)
	{
		MetaSubsystemRef->OnMemoryFragmentsChanged.RemoveDynamic(this, &ThisClass::HandleMemoryFragmentsChanged);
	}
}

void UMortisUpgradeSelectWidget::RebuildEntryList()
{
	if (HB_UpgradeEntries)
	{
		HB_UpgradeEntries->ClearChildren();
	}

	CachedEntries.Reset();
	CreatedEntryWidgets.Reset();

	AddNoUpgradeEntryData();
	AddExperienceEntryData();

	if (!HB_UpgradeEntries || !EntryWidgetClass)
	{
		return;
	}

	for (const FMortisUpgradeSelectEntryData& EntryData : CachedEntries)
	{
		CreateEntryWidget(EntryData);
	}
}

void UMortisUpgradeSelectWidget::AddNoUpgradeEntryData()
{
	FMortisUpgradeSelectEntryData EntryData;
	EntryData.DisplayName = NoUpgradeDisplayName;
	EntryData.Description = NoUpgradeDescription;
	EntryData.Icon = NoUpgradeIcon;
	EntryData.UnlockCost = 0;
	EntryData.bNoUpgradeEntry = true;

	CachedEntries.Add(EntryData);
}

void UMortisUpgradeSelectWidget::AddExperienceEntryData()
{
	if (!ExperienceTableRef)
	{
		return;
	}

	TArray<FMortisExperienceRow*> Rows;
	ExperienceTableRef->GetAllRows(TEXT("UpgradeSelect"), Rows);

	for (const FMortisExperienceRow* Row : Rows)
	{
		if (!Row || !Row->ExperienceTag.IsValid())
		{
			continue;
		}

		FMortisUpgradeSelectEntryData EntryData;
		EntryData.ExperienceTag = Row->ExperienceTag;
		EntryData.DisplayName = Row->DisplayName;
		EntryData.Description = Row->Description;
		EntryData.Icon = Row->Icon;
		EntryData.UnlockCost = Row->UnlockCost;
		EntryData.bNoUpgradeEntry = false;

		CachedEntries.Add(EntryData);
	}
}

UMortisUpgradeIconEntryWidget* UMortisUpgradeSelectWidget::CreateEntryWidget(const FMortisUpgradeSelectEntryData& EntryData)
{
	if (!EntryWidgetClass || !HB_UpgradeEntries)
	{
		return nullptr;
	}

	UMortisUpgradeIconEntryWidget* EntryWidget = CreateWidget<UMortisUpgradeIconEntryWidget>(this, EntryWidgetClass);
	if (!EntryWidget)
	{
		return nullptr;
	}

	EntryWidget->InitializeManualEntry(
		EntryData.ExperienceTag,
		EntryData.DisplayName,
		EntryData.Icon,
		IsEntryUnlocked(EntryData),
		IsEntryActive(EntryData),
		EntryData.bNoUpgradeEntry
	);

	EntryWidget->OnUpgradeEntryClicked.RemoveDynamic(this, &ThisClass::HandleEntryClicked);
	EntryWidget->OnUpgradeEntryClicked.AddDynamic(this, &ThisClass::HandleEntryClicked);

	HB_UpgradeEntries->AddChild(EntryWidget);
	CreatedEntryWidgets.Add(EntryWidget);

	return EntryWidget;
}

void UMortisUpgradeSelectWidget::SelectDefaultEntry()
{
	bHasActiveEntry = false;
	ActiveEntryData = FMortisUpgradeSelectEntryData();

	if (MetaSubsystemRef)
	{
		const FGameplayTag SelectedTag = MetaSubsystemRef->GetSelectedExperienceTag();
		if (SelectedTag.IsValid())
		{
			FMortisUpgradeSelectEntryData SelectedEntry;
			if (FindEntryData(SelectedTag, false, SelectedEntry) && IsEntryUnlocked(SelectedEntry))
			{
				SetActiveEntry(SelectedTag, false);
				return;
			}
		}
	}

	if (SetActiveEntry(FGameplayTag(), true))
	{
		return;
	}

	if (CachedEntries.Num() > 0)
	{
		SetActiveEntry(CachedEntries[0].ExperienceTag, CachedEntries[0].bNoUpgradeEntry);
	}
}

bool UMortisUpgradeSelectWidget::SetActiveEntry(FGameplayTag ExperienceTag, bool bNoUpgradeEntry)
{
	FMortisUpgradeSelectEntryData EntryData;
	if (!FindEntryData(ExperienceTag, bNoUpgradeEntry, EntryData))
	{
		return false;
	}

	ActiveEntryData = EntryData;
	bHasActiveEntry = true;

	RefreshEntryStates();
	RefreshDetailPanel();
	RefreshActionButton();

	return true;
}

bool UMortisUpgradeSelectWidget::FindEntryData(FGameplayTag ExperienceTag, bool bNoUpgradeEntry, FMortisUpgradeSelectEntryData& OutEntryData) const
{
	for (const FMortisUpgradeSelectEntryData& EntryData : CachedEntries)
	{
		if (EntryData.bNoUpgradeEntry != bNoUpgradeEntry)
		{
			continue;
		}

		if (bNoUpgradeEntry || EntryData.ExperienceTag == ExperienceTag)
		{
			OutEntryData = EntryData;
			return true;
		}
	}

	return false;
}

bool UMortisUpgradeSelectWidget::IsEntryActive(const FMortisUpgradeSelectEntryData& EntryData) const
{
	if (!bHasActiveEntry)
	{
		return false;
	}

	if (EntryData.bNoUpgradeEntry || ActiveEntryData.bNoUpgradeEntry)
	{
		return EntryData.bNoUpgradeEntry == ActiveEntryData.bNoUpgradeEntry;
	}

	return EntryData.ExperienceTag == ActiveEntryData.ExperienceTag;
}

bool UMortisUpgradeSelectWidget::IsEntryUnlocked(const FMortisUpgradeSelectEntryData& EntryData) const
{
	if (EntryData.bNoUpgradeEntry)
	{
		return true;
	}

	return MetaSubsystemRef && MetaSubsystemRef->IsExperienceUnlocked(EntryData.ExperienceTag);
}

bool UMortisUpgradeSelectWidget::CanUnlockEntry(const FMortisUpgradeSelectEntryData& EntryData) const
{
	if (EntryData.bNoUpgradeEntry || IsEntryUnlocked(EntryData))
	{
		return false;
	}

	return GetMemoryFragments() >= EntryData.UnlockCost;
}

int32 UMortisUpgradeSelectWidget::GetMemoryFragments() const
{
	return MetaSubsystemRef ? MetaSubsystemRef->GetMemoryFragments() : 0;
}

void UMortisUpgradeSelectWidget::RefreshEntryStates()
{
	for (UMortisUpgradeIconEntryWidget* EntryWidget : CreatedEntryWidgets)
	{
		if (!EntryWidget)
		{
			continue;
		}

		FMortisUpgradeSelectEntryData EntryData;
		if (!FindEntryData(EntryWidget->GetExperienceTag(), EntryWidget->IsNoUpgradeEntry(), EntryData))
		{
			continue;
		}

		EntryWidget->SetUnlocked(IsEntryUnlocked(EntryData));
		EntryWidget->SetActive(IsEntryActive(EntryData));
	}
}

void UMortisUpgradeSelectWidget::RefreshDetailPanel()
{
	if (!bHasActiveEntry)
	{
		ClearDetailPanel();
		return;
	}

	if (TXT_SelectedName)
	{
		TXT_SelectedName->SetText(ActiveEntryData.DisplayName);
	}

	if (TXT_SelectedDescription)
	{
		TXT_SelectedDescription->SetText(ActiveEntryData.Description);
	}

	if (IMG_SelectedIcon)
	{
		if (ActiveEntryData.Icon)
		{
			IMG_SelectedIcon->SetBrushFromTexture(ActiveEntryData.Icon);
			IMG_SelectedIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			IMG_SelectedIcon->SetBrush(FSlateBrush());
			IMG_SelectedIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	const bool bUnlocked = IsEntryUnlocked(ActiveEntryData);
	if (TXT_SelectedState)
	{
		if (ActiveEntryData.bNoUpgradeEntry)
		{
			TXT_SelectedState->SetText(NoUpgradeStateText);
		}
		else if (bUnlocked)
		{
			TXT_SelectedState->SetText(UnlockedStateText);
		}
		else if (CanUnlockEntry(ActiveEntryData))
		{
			TXT_SelectedState->SetText(LockedStateText);
		}
		else
		{
			TXT_SelectedState->SetText(InsufficientFragmentsStateText);
		}
	}

	const bool bShowCost = !ActiveEntryData.bNoUpgradeEntry && !bUnlocked;
	if (HB_Cost)
	{
		HB_Cost->SetVisibility(bShowCost ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (TXT_CostLabel)
	{
		TXT_CostLabel->SetText(CostLabelText);
	}

	if (TXT_CostValue)
	{
		TXT_CostValue->SetText(FText::Format(
			NSLOCTEXT("MortisMetaProgression", "UnlockCostValueFormat", "{0} / {1}"),
			FText::AsNumber(ActiveEntryData.UnlockCost),
			FText::AsNumber(GetMemoryFragments())
		));
	}
}

void UMortisUpgradeSelectWidget::RefreshActionButton()
{
	if (!BTN_Action)
	{
		return;
	}

	if (!bHasActiveEntry)
	{
		BTN_Action->SetIsEnabled(false);
		if (TXT_Action)
		{
			TXT_Action->SetText(StartActionText);
		}
		return;
	}

	const bool bUnlocked = IsEntryUnlocked(ActiveEntryData);
	const bool bCanUseAction = bUnlocked || CanUnlockEntry(ActiveEntryData);

	BTN_Action->SetIsEnabled(bCanUseAction);

	if (TXT_Action)
	{
		TXT_Action->SetText(bUnlocked ? StartActionText : UnlockActionText);
	}
}

bool UMortisUpgradeSelectWidget::TryUnlockActiveEntry()
{
	if (!bHasActiveEntry || ActiveEntryData.bNoUpgradeEntry || !MetaSubsystemRef)
	{
		return false;
	}

	if (!MetaSubsystemRef->UnlockExperience(ActiveEntryData.ExperienceTag))
	{
		RefreshDetailPanel();
		RefreshActionButton();
		return false;
	}

	RefreshEntryStates();
	RefreshDetailPanel();
	RefreshActionButton();

	return true;
}

void UMortisUpgradeSelectWidget::StartRunWithActiveEntry()
{
	if (!bHasActiveEntry)
	{
		return;
	}

	if (ActiveEntryData.bNoUpgradeEntry)
	{
		OnRunStartRequested.Broadcast(FGameplayTag(), true);
		return;
	}

	if (!MetaSubsystemRef || !IsEntryUnlocked(ActiveEntryData))
	{
		return;
	}

	if (MetaSubsystemRef->SelectExperience(ActiveEntryData.ExperienceTag))
	{
		OnRunStartRequested.Broadcast(ActiveEntryData.ExperienceTag, false);
	}
}

void UMortisUpgradeSelectWidget::ClearDetailPanel()
{
	if (TXT_SelectedName)
	{
		TXT_SelectedName->SetText(FText::GetEmpty());
	}

	if (TXT_SelectedState)
	{
		TXT_SelectedState->SetText(FText::GetEmpty());
	}

	if (TXT_SelectedDescription)
	{
		TXT_SelectedDescription->SetText(FText::GetEmpty());
	}

	if (IMG_SelectedIcon)
	{
		IMG_SelectedIcon->SetBrush(FSlateBrush());
		IMG_SelectedIcon->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (HB_Cost)
	{
		HB_Cost->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (BTN_Action)
	{
		BTN_Action->SetIsEnabled(false);
	}
}
