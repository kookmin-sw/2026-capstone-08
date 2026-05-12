#include "UI/MortisUpgradeSelectWidget.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
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

	if (TXT_UnlockUpgrade)
	{
		TXT_UnlockUpgrade->SetText(UnlockActionText);
	}

	if (TXT_StartGame)
	{
		TXT_StartGame->SetText(StartActionText);
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
	RefreshActionButtons();
}

void UMortisUpgradeSelectWidget::RefreshFocus()
{
	if (BTN_StartGame && BTN_StartGame->GetIsEnabled())
	{
		BTN_StartGame->SetKeyboardFocus();
		return;
	}

	if (BTN_UnlockUpgrade && BTN_UnlockUpgrade->GetIsEnabled())
	{
		BTN_UnlockUpgrade->SetKeyboardFocus();
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

void UMortisUpgradeSelectWidget::HandleUnlockUpgradeClicked()
{
	TryUnlockActiveEntry();
}

void UMortisUpgradeSelectWidget::HandleStartGameClicked()
{
	StartRunWithActiveEntry();
}

void UMortisUpgradeSelectWidget::HandleMemoryFragmentsChanged(int32 NewValue)
{
	RefreshDetailPanel();
	RefreshActionButtons();
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
	if (BTN_UnlockUpgrade)
	{
		BTN_UnlockUpgrade->OnClicked.RemoveDynamic(this, &ThisClass::HandleUnlockUpgradeClicked);
		BTN_UnlockUpgrade->OnClicked.AddDynamic(this, &ThisClass::HandleUnlockUpgradeClicked);
	}

	if (BTN_StartGame)
	{
		BTN_StartGame->OnClicked.RemoveDynamic(this, &ThisClass::HandleStartGameClicked);
		BTN_StartGame->OnClicked.AddDynamic(this, &ThisClass::HandleStartGameClicked);
	}
}

void UMortisUpgradeSelectWidget::UnbindWidgetEvents()
{
	if (BTN_UnlockUpgrade)
	{
		BTN_UnlockUpgrade->OnClicked.RemoveDynamic(this, &ThisClass::HandleUnlockUpgradeClicked);
	}

	if (BTN_StartGame)
	{
		BTN_StartGame->OnClicked.RemoveDynamic(this, &ThisClass::HandleStartGameClicked);
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
	if (UGP_UpgradeEntries)
	{
		UGP_UpgradeEntries->ClearChildren();
	}

	CachedEntries.Reset();
	CreatedEntryWidgets.Reset();

	AddNoUpgradeEntryData();
	AddExperienceEntryData();

	if (!UGP_UpgradeEntries || !EntryWidgetClass)
	{
		return;
	}

	for (int32 EntryIndex = 0; EntryIndex < CachedEntries.Num(); ++EntryIndex)
	{
		CreateEntryWidget(CachedEntries[EntryIndex], EntryIndex);
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

UMortisUpgradeIconEntryWidget* UMortisUpgradeSelectWidget::CreateEntryWidget(const FMortisUpgradeSelectEntryData& EntryData, int32 EntryIndex)
{
	if (!EntryWidgetClass || !UGP_UpgradeEntries)
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

	const int32 SafeColumnCount = FMath::Max(1, UpgradeGridColumns);
	const int32 Row = EntryIndex / SafeColumnCount;
	const int32 Column = EntryIndex % SafeColumnCount;

	UGP_UpgradeEntries->AddChildToUniformGrid(EntryWidget, Row, Column);
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
	RefreshActionButtons();

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

void UMortisUpgradeSelectWidget::RefreshActionButtons()
{
	if (!bHasActiveEntry)
	{
		if (BTN_UnlockUpgrade)
		{
			BTN_UnlockUpgrade->SetVisibility(ESlateVisibility::Collapsed);
			BTN_UnlockUpgrade->SetIsEnabled(false);
		}

		if (BTN_StartGame)
		{
			BTN_StartGame->SetIsEnabled(false);
		}

		if (TXT_UnlockUpgrade)
		{
			TXT_UnlockUpgrade->SetText(UnlockActionText);
		}

		if (TXT_StartGame)
		{
			TXT_StartGame->SetText(StartActionText);
		}
		return;
	}

	const bool bUnlocked = IsEntryUnlocked(ActiveEntryData);
	const bool bCanUnlock = CanUnlockEntry(ActiveEntryData);
	const bool bShowUnlock = !ActiveEntryData.bNoUpgradeEntry && !bUnlocked;

	if (BTN_UnlockUpgrade)
	{
		BTN_UnlockUpgrade->SetVisibility(bShowUnlock ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		BTN_UnlockUpgrade->SetIsEnabled(bShowUnlock && bCanUnlock);
	}

	if (BTN_StartGame)
	{
		BTN_StartGame->SetIsEnabled(bUnlocked);
	}

	if (TXT_UnlockUpgrade)
	{
		TXT_UnlockUpgrade->SetText(UnlockActionText);
	}

	if (TXT_StartGame)
	{
		TXT_StartGame->SetText(StartActionText);
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
		RefreshActionButtons();
		return false;
	}

	RefreshEntryStates();
	RefreshDetailPanel();
	RefreshActionButtons();

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
		PrintStartRequestPlaceholder(FGameplayTag(), true);
		OnRunStartRequested.Broadcast(FGameplayTag(), true);
		return;
	}

	if (!MetaSubsystemRef || !IsEntryUnlocked(ActiveEntryData))
	{
		return;
	}

	if (MetaSubsystemRef->SelectExperience(ActiveEntryData.ExperienceTag))
	{
		PrintStartRequestPlaceholder(ActiveEntryData.ExperienceTag, false);
		OnRunStartRequested.Broadcast(ActiveEntryData.ExperienceTag, false);
	}
}

void UMortisUpgradeSelectWidget::PrintStartRequestPlaceholder(FGameplayTag SelectedExperienceTag, bool bNoUpgrade) const
{
	const FString PrintMessage = bNoUpgrade
		? TEXT("Start Requested: No Upgrade")
		: FString::Printf(TEXT("Start Requested: %s"), *SelectedExperienceTag.ToString());

	UKismetSystemLibrary::PrintString(
		this,
		PrintMessage,
		true,
		true,
		FLinearColor(0.0f, 1.0f, 1.0f, 1.0f),
		2.0f
	);
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

	if (BTN_UnlockUpgrade)
	{
		BTN_UnlockUpgrade->SetVisibility(ESlateVisibility::Collapsed);
		BTN_UnlockUpgrade->SetIsEnabled(false);
	}

	if (BTN_StartGame)
	{
		BTN_StartGame->SetIsEnabled(false);
	}
}
