#include "UI/MortisInventoryRunePageWidget.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "System/MortisRuneDatabaseSubsystem.h"
#include "System/MortisGameDataSettings.h"
#include "System/MortisRuneInventorySubsystem.h"
#include "UI/MortisRuneCardWidget.h"
#include "UI/MortisRunePresentationStyle.h"
#include "UI/MortisRightRuneDetailWidget.h"
#include "UI/MortisRuneSetEntryWidget.h"

void UMortisInventoryRunePageWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeReferences();
    BindInventoryDelegates();

    if (Widget_RightRuneDetail)
    {
        Widget_RightRuneDetail->OnEquipRequested.RemoveDynamic(this, &ThisClass::HandleEquipButtonClicked);
        Widget_RightRuneDetail->OnEquipRequested.AddDynamic(this, &ThisClass::HandleEquipButtonClicked);
    }

    RefreshAll();
}

void UMortisInventoryRunePageWidget::NativeDestruct()
{
    if (Widget_RightRuneDetail)
    {
        Widget_RightRuneDetail->OnEquipRequested.RemoveDynamic(this, &ThisClass::HandleEquipButtonClicked);
    }

    UnbindInventoryDelegates();

    Super::NativeDestruct();
}

void UMortisInventoryRunePageWidget::RefreshAll()
{
    RefreshSetList();
    UpdateCenterState();
    RefreshEquippedRuneGrid();
    RefreshRuneGrid();
    RefreshRightPanel();
}

void UMortisInventoryRunePageWidget::HandleSetEntryClicked(FGameplayTag ClickedSetTag)
{
    SelectedSetTag = (SelectedSetTag == ClickedSetTag) ? FGameplayTag() : ClickedSetTag;
    SelectedRune = FMortisRuneInstance();

    RefreshAll();
}

void UMortisInventoryRunePageWidget::HandleRuneCardClicked(FMortisRuneInstance ClickedRune)
{
    SelectedRune = ClickedRune;

    RefreshRuneGrid();
    RefreshRightPanel();
}

void UMortisInventoryRunePageWidget::HandleEquipButtonClicked()
{
    if (!RuneInventorySubsystemRef || !HasSelectedRune())
    {
        return;
    }

    if (IsRuneEquipped(SelectedRune.InstanceId))
    {
        return;
    }

    const int32 TargetSlotIndex = FindFirstEmptyRuneSlot();
    if (TargetSlotIndex == INDEX_NONE)
    {
        RefreshRightPanel();
        return;
    }

    RuneInventorySubsystemRef->EquipRune(TargetSlotIndex, SelectedRune);
}

void UMortisInventoryRunePageWidget::HandleOwningRuneAdded(const FMortisRuneInstance& AddedRune)
{
    RefreshAll();
}

void UMortisInventoryRunePageWidget::HandleOwningRuneRemoved(const FMortisRuneInstance& RemovedRune)
{
    RefreshAll();
}

void UMortisInventoryRunePageWidget::HandleEquippedRuneAdded(int32 SlotIndex, const FMortisRuneInstance& AddedRune)
{
    RefreshAll();
}

void UMortisInventoryRunePageWidget::HandleEquippedRuneRemoved(int32 SlotIndex, const FMortisRuneInstance& RemovedRune)
{
    RefreshAll();
}

void UMortisInventoryRunePageWidget::InitializeReferences()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        RuneInventorySubsystemRef = GameInstance->GetSubsystem<UMortisRuneInventorySubsystem>();
        RuneDatabaseSubsystemRef = GameInstance->GetSubsystem<UMortisRuneDatabaseSubsystem>();
    }

    const UMortisGameDataSettings* Settings = GetDefault<UMortisGameDataSettings>();
    if (Settings)
    {
        RuneSetTableRef = Settings->RuneSetTable.LoadSynchronous();
    }
}

void UMortisInventoryRunePageWidget::BindInventoryDelegates()
{
    if (!RuneInventorySubsystemRef)
    {
        return;
    }

    RuneInventorySubsystemRef->OnOwningRuneAdded.RemoveDynamic(this, &ThisClass::HandleOwningRuneAdded);
    RuneInventorySubsystemRef->OnOwningRuneRemoved.RemoveDynamic(this, &ThisClass::HandleOwningRuneRemoved);
    RuneInventorySubsystemRef->OnEquippedRuneAdded.RemoveDynamic(this, &ThisClass::HandleEquippedRuneAdded);
    RuneInventorySubsystemRef->OnEquippedRuneRemoved.RemoveDynamic(this, &ThisClass::HandleEquippedRuneRemoved);

    RuneInventorySubsystemRef->OnOwningRuneAdded.AddDynamic(this, &ThisClass::HandleOwningRuneAdded);
    RuneInventorySubsystemRef->OnOwningRuneRemoved.AddDynamic(this, &ThisClass::HandleOwningRuneRemoved);
    RuneInventorySubsystemRef->OnEquippedRuneAdded.AddDynamic(this, &ThisClass::HandleEquippedRuneAdded);
    RuneInventorySubsystemRef->OnEquippedRuneRemoved.AddDynamic(this, &ThisClass::HandleEquippedRuneRemoved);
}

void UMortisInventoryRunePageWidget::UnbindInventoryDelegates()
{
    if (!RuneInventorySubsystemRef)
    {
        return;
    }

    RuneInventorySubsystemRef->OnOwningRuneAdded.RemoveDynamic(this, &ThisClass::HandleOwningRuneAdded);
    RuneInventorySubsystemRef->OnOwningRuneRemoved.RemoveDynamic(this, &ThisClass::HandleOwningRuneRemoved);
    RuneInventorySubsystemRef->OnEquippedRuneAdded.RemoveDynamic(this, &ThisClass::HandleEquippedRuneAdded);
    RuneInventorySubsystemRef->OnEquippedRuneRemoved.RemoveDynamic(this, &ThisClass::HandleEquippedRuneRemoved);
}

void UMortisInventoryRunePageWidget::RefreshSetList()
{
    if (!ScrollBox_SetList)
    {
        return;
    }

    ScrollBox_SetList->ClearChildren();
    CreatedSetEntries.Reset();

    if (!RuneSetTableRef || !RuneSetEntryClass)
    {
        return;
    }

    TArray<FMortisRuneSetRow*> AllSetRows;
    RuneSetTableRef->GetAllRows(TEXT("InventoryRunePage"), AllSetRows);

    for (const FMortisRuneSetRow* SetRow : AllSetRows)
    {
        if (!SetRow)
        {
            continue;
        }

        UMortisRuneSetEntryWidget* SetEntry = CreateWidget<UMortisRuneSetEntryWidget>(this, RuneSetEntryClass);
        if (!SetEntry)
        {
            continue;
        }

        UTexture2D* DisplayIcon = SetRow->ListIcon;
        const int32 OwnedCount = GetTotalOwnedCountBySetTag(SetRow->SetTag);
        const bool bIsSelected = SelectedSetTag.IsValid() && SelectedSetTag == SetRow->SetTag;

        SetEntry->ApplyData(SetRow->SetTag, DisplayIcon, SetRow->SetName, OwnedCount, bIsSelected);
        SetEntry->OnSetEntryClicked.RemoveDynamic(this, &ThisClass::HandleSetEntryClicked);
        SetEntry->OnSetEntryClicked.AddDynamic(this, &ThisClass::HandleSetEntryClicked);

        ScrollBox_SetList->AddChild(SetEntry);
        CreatedSetEntries.Add(SetEntry);
    }
}

void UMortisInventoryRunePageWidget::UpdateCenterState()
{
    if (WidgetSwitcher_CenterState)
    {
        WidgetSwitcher_CenterState->SetActiveWidgetIndex(SelectedSetTag.IsValid() ? 1 : 0);
    }

    if (Text_EmptyMessage && !SelectedSetTag.IsValid())
    {
        Text_EmptyMessage->SetText(NSLOCTEXT("MortisInventory", "SelectRuneSetMessage", "Select a rune set."));
    }

    UpdateCenterHeader();
}

void UMortisInventoryRunePageWidget::UpdateCenterHeader()
{
    if (Text_SelectedSetName)
    {
        if (!SelectedSetTag.IsValid())
        {
            Text_SelectedSetName->SetText(NSLOCTEXT("MortisInventory", "EquippedRunesHeader", "Equipped Runes"));
        }
        else if (RuneDatabaseSubsystemRef)
        {
            const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef->GetRuneSetRow(SelectedSetTag);
            Text_SelectedSetName->SetText(SetRow ? SetRow->SetName : FText::GetEmpty());
        }
    }

    if (Text_RuneCount)
    {
        if (!SelectedSetTag.IsValid())
        {
            const int32 SlotCount = RuneInventorySubsystemRef ? RuneInventorySubsystemRef->GetSlotCount() : 0;
            Text_RuneCount->SetText(FText::Format(
                NSLOCTEXT("MortisInventory", "EquippedRuneCountFormat", "{0} / {1} Equipped"),
                FText::AsNumber(GetEquippedRuneCount()),
                FText::AsNumber(SlotCount)));
        }
        else
        {
            TArray<FMortisRuneInstance> MatchingRunes;
            GetOwningRunesBySetTag(SelectedSetTag, MatchingRunes);
            Text_RuneCount->SetText(FText::Format(
                NSLOCTEXT("MortisInventory", "RuneCountFormat", "{0} Runes"),
                FText::AsNumber(MatchingRunes.Num())));
        }
    }
}

void UMortisInventoryRunePageWidget::RefreshEquippedRuneGrid()
{
    ClearEquippedRuneGrid();

    if (SelectedSetTag.IsValid())
    {
        return;
    }

    UpdateCenterHeader();

    if (Text_EmptyMessage)
    {
        Text_EmptyMessage->SetText(NSLOCTEXT("MortisInventory", "OverviewHintMessage", "Select a rune set to browse owned runes."));
    }

    if (!UniformGrid_EquippedRuneGrid || !RuneCardClass || !RuneInventorySubsystemRef)
    {
        return;
    }

    const TArray<FMortisRuneInstance>& EquippedRunes = RuneInventorySubsystemRef->GetEquippedRunes();
    const int32 SafeColumns = FMath::Max(1, EquippedRuneColumns);

    if (HasSelectedRune())
    {
        const bool bSelectedRuneStillExists = EquippedRunes.ContainsByPredicate(
            [this](const FMortisRuneInstance& Rune)
            {
                return Rune.InstanceId.IsValid() && Rune.InstanceId == SelectedRune.InstanceId;
            });

        if (!bSelectedRuneStillExists)
        {
            SelectedRune = FMortisRuneInstance();
        }
    }

    int32 VisibleIndex = 0;

    for (const FMortisRuneInstance& RuneInstance : EquippedRunes)
    {
        if (!RuneInstance.InstanceId.IsValid())
        {
            continue;
        }

        UMortisRuneCardWidget* RuneCard = CreateWidget<UMortisRuneCardWidget>(this, RuneCardClass);
        if (!RuneCard)
        {
            continue;
        }

        const bool bIsSelected = HasSelectedRune() && SelectedRune.InstanceId == RuneInstance.InstanceId;
        ConfigureRuneCard(RuneCard, RuneInstance, bIsSelected, true);

        const int32 Row = VisibleIndex / SafeColumns;
        const int32 Column = VisibleIndex % SafeColumns;

        if (UUniformGridSlot* GridSlot = UniformGrid_EquippedRuneGrid->AddChildToUniformGrid(RuneCard, Row, Column))
        {
            //GridSlot->SetHorizontalAlignment(HAlign_Fill);
            //GridSlot->SetVerticalAlignment(VAlign_Fill);
        }

        CreatedEquippedRuneCards.Add(RuneCard);
        ++VisibleIndex;
    }
}

void UMortisInventoryRunePageWidget::RefreshRuneGrid()
{
    ClearRuneGrid();

    if (!SelectedSetTag.IsValid())
    {
        return;
    }

    UpdateCenterHeader();

    if (!UniformGrid_RuneGrid || !RuneCardClass || !RuneDatabaseSubsystemRef)
    {
        return;
    }

    TArray<FMortisRuneInstance> MatchingRunes;
    GetOwningRunesBySetTag(SelectedSetTag, MatchingRunes);

    if (HasSelectedRune())
    {
        const bool bStillExists = MatchingRunes.ContainsByPredicate(
            [this](const FMortisRuneInstance& Rune)
            {
                return Rune.InstanceId == SelectedRune.InstanceId;
            });

        if (!bStillExists)
        {
            SelectedRune = FMortisRuneInstance();
        }
    }

    const int32 SafeColumns = FMath::Max(1, RuneColumns);

    for (int32 Index = 0; Index < MatchingRunes.Num(); ++Index)
    {
        const FMortisRuneInstance& Rune = MatchingRunes[Index];

        UMortisRuneCardWidget* RuneCard = CreateWidget<UMortisRuneCardWidget>(this, RuneCardClass);
        if (!RuneCard)
        {
            continue;
        }

        const bool bIsSelected = HasSelectedRune() && SelectedRune.InstanceId == Rune.InstanceId;
        const bool bIsEquipped = IsRuneEquipped(Rune.InstanceId);

        ConfigureRuneCard(RuneCard, Rune, bIsSelected, bIsEquipped);

        const int32 Row = Index / SafeColumns;
        const int32 Column = Index % SafeColumns;

        if (UUniformGridSlot* GridSlot = UniformGrid_RuneGrid->AddChildToUniformGrid(RuneCard, Row, Column))
        {
            // GridSlot->SetHorizontalAlignment(HAlign_Fill);
            // GridSlot->SetVerticalAlignment(VAlign_Fill);
        }

        CreatedRuneCards.Add(RuneCard);
    }
}

void UMortisInventoryRunePageWidget::RefreshRightPanel()
{
    if (!WidgetSwitcher_RightState)
    {
        return;
    }

    if (HasSelectedRune())
    {
        WidgetSwitcher_RightState->SetActiveWidgetIndex(2);
        UpdateRightRuneDetail();
        return;
    }

    if (!SelectedSetTag.IsValid())
    {
        WidgetSwitcher_RightState->SetActiveWidgetIndex(0);
        UpdateRightOverview();
        return;
    }

    if (!HasSelectedRune())
    {
        WidgetSwitcher_RightState->SetActiveWidgetIndex(1);
        UpdateRightSetDetail();
        return;
    }

    WidgetSwitcher_RightState->SetActiveWidgetIndex(2);
    UpdateRightRuneDetail();
}

void UMortisInventoryRunePageWidget::ClearEquippedRuneGrid()
{
    CreatedEquippedRuneCards.Reset();

    if (UniformGrid_EquippedRuneGrid)
    {
        UniformGrid_EquippedRuneGrid->ClearChildren();
    }
}

void UMortisInventoryRunePageWidget::ClearRuneGrid()
{
    CreatedRuneCards.Reset();

    if (UniformGrid_RuneGrid)
    {
        UniformGrid_RuneGrid->ClearChildren();
    }
}

void UMortisInventoryRunePageWidget::ConfigureRuneCard(UMortisRuneCardWidget* RuneCard, const FMortisRuneInstance& RuneInstance, bool bInSelected, bool bInEquipped)
{
    if (!RuneCard)
    {
        return;
    }

    UTexture2D* DisplayIcon = nullptr;
    FLinearColor DisplayIconTint = FLinearColor::White;

    if (RuneDatabaseSubsystemRef)
    {
        const FMortisRuneSymbolRow* SymbolRow = RuneDatabaseSubsystemRef->GetRuneSymbolRow(RuneInstance.SymbolType);
        DisplayIcon = SymbolRow ? SymbolRow->Glyph : nullptr;
        DisplayIconTint = GetGlyphTintBySetTag(RuneInstance.SetTag);
    }

    RuneCard->ClearRuneData();
    RuneCard->SetRuneInstance(RuneInstance);
    RuneCard->SetDisplayIcon(DisplayIcon);
    RuneCard->SetDisplayIconTint(DisplayIconTint);
    RuneCard->SetSelected(bInSelected);
    RuneCard->SetEquipped(bInEquipped);
    RuneCard->RefreshVisualState();
    RuneCard->OnRuneCardClicked.RemoveDynamic(this, &ThisClass::HandleRuneCardClicked);
    RuneCard->OnRuneCardClicked.AddDynamic(this, &ThisClass::HandleRuneCardClicked);
    RuneCard->SetIsEnabled(true);
}

int32 UMortisInventoryRunePageWidget::GetTotalOwnedCountBySetTag(const FGameplayTag& SetTag) const
{
    if (!RuneInventorySubsystemRef || !SetTag.IsValid())
    {
        return 0;
    }

    int32 Count = 0;

    for (const FMortisRuneInstance& Rune : RuneInventorySubsystemRef->GetOwningRunes())
    {
        if (Rune.SetTag == SetTag)
        {
            ++Count;
        }
    }

    for (const FMortisRuneInstance& Rune : RuneInventorySubsystemRef->GetEquippedRunes())
    {
        if (Rune.InstanceId.IsValid() && Rune.SetTag == SetTag)
        {
            ++Count;
        }
    }

    return Count;
}

void UMortisInventoryRunePageWidget::GetOwningRunesBySetTag(const FGameplayTag& SetTag, TArray<FMortisRuneInstance>& OutRunes) const
{
    OutRunes.Reset();

    if (!RuneInventorySubsystemRef || !SetTag.IsValid())
    {
        return;
    }

    for (const FMortisRuneInstance& Rune : RuneInventorySubsystemRef->GetOwningRunes())
    {
        if (Rune.SetTag == SetTag)
        {
            OutRunes.Add(Rune);
        }
    }
}

bool UMortisInventoryRunePageWidget::IsRuneEquipped(const FGuid& RuneId) const
{
    if (!RuneInventorySubsystemRef || !RuneId.IsValid())
    {
        return false;
    }

    for (const FMortisRuneInstance& Rune : RuneInventorySubsystemRef->GetEquippedRunes())
    {
        if (Rune.InstanceId == RuneId)
        {
            return true;
        }
    }

    return false;
}

bool UMortisInventoryRunePageWidget::HasSelectedRune() const
{
    return SelectedRune.InstanceId.IsValid();
}

int32 UMortisInventoryRunePageWidget::GetEquippedRuneCount() const
{
    if (!RuneInventorySubsystemRef)
    {
        return 0;
    }

    int32 EquippedCount = 0;

    for (const FMortisRuneInstance& Rune : RuneInventorySubsystemRef->GetEquippedRunes())
    {
        if (Rune.InstanceId.IsValid())
        {
            ++EquippedCount;
        }
    }

    return EquippedCount;
}

int32 UMortisInventoryRunePageWidget::GetEquippedCountBySetTag(const FGameplayTag& SetTag) const
{
    if (!RuneInventorySubsystemRef || !SetTag.IsValid())
    {
        return 0;
    }

    int32 EquippedCount = 0;

    for (const FMortisRuneInstance& Rune : RuneInventorySubsystemRef->GetEquippedRunes())
    {
        if (Rune.InstanceId.IsValid() && Rune.SetTag == SetTag)
        {
            ++EquippedCount;
        }
    }

    return EquippedCount;
}

FLinearColor UMortisInventoryRunePageWidget::GetGlyphTintBySetTag(const FGameplayTag& SetTag) const
{
    if (!RuneDatabaseSubsystemRef || !SetTag.IsValid())
    {
        return FLinearColor::White;
    }

    const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef->GetRuneSetRow(SetTag);
    return SetRow ? SetRow->GlyphTint : FLinearColor::White;
}

void UMortisInventoryRunePageWidget::UpdateRightOverview()
{
    if (Text_RightOverviewTitle)
    {
        Text_RightOverviewTitle->SetText(NSLOCTEXT("MortisInventory", "RightOverviewTitle", "Rune Overview"));
    }

    if (Text_EquippedSummaryTitle)
    {
        Text_EquippedSummaryTitle->SetText(NSLOCTEXT("MortisInventory", "EquippedSummaryTitle", "Equipped Summary"));
    }

    if (Text_EquippedSummaryBody)
    {
        Text_EquippedSummaryBody->SetText(BuildEquippedSummaryText());
    }

    if (Text_ActiveSetSummaryTitle)
    {
        Text_ActiveSetSummaryTitle->SetText(NSLOCTEXT("MortisInventory", "ActiveSetSummaryTitle", "Active Set Effects"));
    }

    if (Text_ActiveSetSummaryBody)
    {
        Text_ActiveSetSummaryBody->SetText(BuildActiveSetSummaryText());
    }

}

void UMortisInventoryRunePageWidget::UpdateRightSetDetail()
{
    if (Text_RightSetTitle)
    {
        Text_RightSetTitle->SetText(NSLOCTEXT("MortisInventory", "RightSetTitle", "Set Details"));
    }

    const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef
        ? RuneDatabaseSubsystemRef->GetRuneSetRow(SelectedSetTag)
        : nullptr;

    if (Text_SelectedSetName_Right)
    {
        Text_SelectedSetName_Right->SetText(SetRow ? SetRow->SetName : FText::GetEmpty());
    }

    if (Text_SelectedSetCount)
    {
        Text_SelectedSetCount->SetText(BuildSelectedSetCountText());
    }

    if (Text_SetEffectTier01)
    {
        Text_SetEffectTier01->SetText(FText::GetEmpty());
    }

    if (Text_SetEffectTier02)
    {
        Text_SetEffectTier02->SetText(FText::GetEmpty());
    }

    if (Text_SetEffectTier03)
    {
        Text_SetEffectTier03->SetText(FText::GetEmpty());
    }

    if (SetRow)
    {
        if (SetRow->TierDefs.Num() > 0 && Text_SetEffectTier01)
        {
            Text_SetEffectTier01->SetText(FText::Format(
                NSLOCTEXT("MortisInventory", "SetTier01", "{0}-piece effect"),
                FText::AsNumber(SetRow->TierDefs[0].ActivateCount)));
        }

        if (SetRow->TierDefs.Num() > 1 && Text_SetEffectTier02)
        {
            Text_SetEffectTier02->SetText(FText::Format(
                NSLOCTEXT("MortisInventory", "SetTier02", "{0}-piece effect"),
                FText::AsNumber(SetRow->TierDefs[1].ActivateCount)));
        }

        if (SetRow->TierDefs.Num() > 2 && Text_SetEffectTier03)
        {
            Text_SetEffectTier03->SetText(FText::Format(
                NSLOCTEXT("MortisInventory", "SetTier03", "{0}-piece effect"),
                FText::AsNumber(SetRow->TierDefs[2].ActivateCount)));
        }
    }

}

void UMortisInventoryRunePageWidget::UpdateRightRuneDetail()
{
    if (!Widget_RightRuneDetail || !HasSelectedRune())
    {
        return;
    }

    FMortisRightRuneDetailViewData ViewData;
    const FGameplayTag DetailSetTag = SelectedRune.SetTag;
    const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef
        ? RuneDatabaseSubsystemRef->GetRuneSetRow(DetailSetTag)
        : nullptr;
    const FMortisRuneSymbolRow* SymbolRow = RuneDatabaseSubsystemRef
        ? RuneDatabaseSubsystemRef->GetRuneSymbolRow(SelectedRune.SymbolType)
        : nullptr;

    ViewData.RuneIcon = SymbolRow ? SymbolRow->Glyph : nullptr;
    ViewData.RuneIconTint = GetGlyphTintBySetTag(DetailSetTag);
    ViewData.RunePresentationStyle = MortisRunePresentation::BuildStyle(SelectedRune.Grade, ViewData.RuneIconTint);
    ViewData.RuneName = GetSelectedRuneNameText();
    ViewData.RuneStatValue = BuildRuneStatValueText();
    ViewData.SetName = SetRow ? SetRow->SetName : FText::GetEmpty();
    ViewData.SetProgress = BuildSetProgressTextBySetTag(DetailSetTag);
    ViewData.bShowEquipAction = !IsRuneEquipped(SelectedRune.InstanceId);
    ViewData.bCanEquip = CanEquipSelectedRune();
    ViewData.EquipLabel = GetEquipButtonLabelText();
    BuildSelectedRuneSynergyEntries(DetailSetTag, ViewData.SynergyEntries);

    Widget_RightRuneDetail->ApplyData(ViewData);
}

FText UMortisInventoryRunePageWidget::BuildEquippedSummaryText() const
{
    if (!RuneInventorySubsystemRef)
    {
        return FText::GetEmpty();
    }

    const int32 SlotCount = RuneInventorySubsystemRef->GetSlotCount();

    return FText::Format(
        NSLOCTEXT("MortisInventory", "EquippedSummaryFormat", "{0} / {1} equipped"),
        FText::AsNumber(GetEquippedRuneCount()),
        FText::AsNumber(SlotCount));
}

FText UMortisInventoryRunePageWidget::BuildActiveSetSummaryText() const
{
    if (!RuneInventorySubsystemRef || !RuneDatabaseSubsystemRef)
    {
        return FText::GetEmpty();
    }

    const TArray<FGameplayTag>& ActiveSetTags = RuneInventorySubsystemRef->GetActiveRuneSets();
    if (ActiveSetTags.IsEmpty())
    {
        return NSLOCTEXT("MortisInventory", "NoActiveSetEffects", "No active set effects");
    }

    FString Summary;

    for (const FGameplayTag& ActiveSetTag : ActiveSetTags)
    {
        const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef->GetRuneSetRow(ActiveSetTag);
        FMortisActiveRuneSetState SetState;
        const bool bHasState = RuneInventorySubsystemRef->GetRuneSetStateByTag(ActiveSetTag, SetState);

        const FString SetName = SetRow ? SetRow->SetName.ToString() : ActiveSetTag.ToString();
        const int32 ActiveLevel = bHasState ? SetState.CurrentLevel : 0;

        if (!Summary.IsEmpty())
        {
            Summary.Append(TEXT("\n"));
        }

        Summary.Append(FString::Printf(TEXT("%s Lv.%d"), *SetName, ActiveLevel));
    }

    return FText::FromString(Summary);
}

FText UMortisInventoryRunePageWidget::BuildSelectedSetCountText() const
{
    if (!RuneInventorySubsystemRef || !SelectedSetTag.IsValid())
    {
        return FText::GetEmpty();
    }

    int32 OwnedCount = 0;
    int32 EquippedCount = 0;

    for (const FMortisRuneInstance& Rune : RuneInventorySubsystemRef->GetOwningRunes())
    {
        if (Rune.SetTag == SelectedSetTag)
        {
            ++OwnedCount;
        }
    }

    for (const FMortisRuneInstance& Rune : RuneInventorySubsystemRef->GetEquippedRunes())
    {
        if (Rune.InstanceId.IsValid() && Rune.SetTag == SelectedSetTag)
        {
            ++OwnedCount;
            ++EquippedCount;
        }
    }

    return FText::Format(
        NSLOCTEXT("MortisInventory", "SelectedSetCountFormat", "Owned {0} / Equipped {1}"),
        FText::AsNumber(OwnedCount),
        FText::AsNumber(EquippedCount));
}

FText UMortisInventoryRunePageWidget::BuildSetProgressTextBySetTag(const FGameplayTag& SetTag) const
{
    if (!RuneDatabaseSubsystemRef || !SetTag.IsValid())
    {
        return FText::GetEmpty();
    }

    const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef->GetRuneSetRow(SetTag);
    if (!SetRow)
    {
        return FText::GetEmpty();
    }

    const int32 CurrentEquippedCount = GetEquippedCountBySetTag(SetTag);

    for (const FMortisSetTierDef& TierDef : SetRow->TierDefs)
    {
        if (CurrentEquippedCount < TierDef.ActivateCount)
        {
            return FText::Format(
                NSLOCTEXT("MortisInventory", "SetProgressNextTierFormat", "현재 장착 {0} / 다음 효과까지 {1}"),
                FText::AsNumber(CurrentEquippedCount),
                FText::AsNumber(TierDef.ActivateCount));
        }
    }

    return FText::Format(
        NSLOCTEXT("MortisInventory", "SetProgressCompletedFormat", "현재 장착 {0} / 모든 효과 활성화"),
        FText::AsNumber(CurrentEquippedCount));
}

FText UMortisInventoryRunePageWidget::BuildRuneStatValueText() const
{
    if (!HasSelectedRune() || !RuneDatabaseSubsystemRef)
    {
        return FText::GetEmpty();
    }

    return FText::Format(
        NSLOCTEXT("MortisInventory", "RuneStatValueOnlyFormat", "+{0}"),
        FText::AsNumber(FMath::RoundToInt(SelectedRune.RolledValue)));
}

FText UMortisInventoryRunePageWidget::GetSelectedRuneNameText() const
{
    if (!HasSelectedRune() || !RuneDatabaseSubsystemRef)
    {
        return FText::GetEmpty();
    }

    const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef->GetRuneSetRow(SelectedRune.SetTag);
    const FMortisRuneSymbolRow* SymbolRow = RuneDatabaseSubsystemRef->GetRuneSymbolRow(SelectedRune.SymbolType);

    if (SymbolRow)
    {
        return FText::Format(
            NSLOCTEXT("MortisInventory", "SelectedRuneNameFormat", "{0} Rune"),
            SymbolRow->DisplayName);
    }

    if (SetRow)
    {
        return SetRow->SetName;
    }

    return FText::FromString(SelectedRune.SetTag.ToString());
}

void UMortisInventoryRunePageWidget::BuildSelectedRuneSynergyEntries(const FGameplayTag& SetTag, TArray<FMortisRuneSynergyEntryViewData>& OutEntries) const
{
    OutEntries.Reset();

    if (!RuneDatabaseSubsystemRef || !SetTag.IsValid())
    {
        return;
    }

    const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef->GetRuneSetRow(SetTag);
    if (!SetRow)
    {
        return;
    }

    const int32 CurrentEquippedCount = GetEquippedCountBySetTag(SetTag);

    for (int32 TierIndex = 0; TierIndex < SetRow->TierDefs.Num(); ++TierIndex)
    {
        const FMortisSetTierDef& TierDef = SetRow->TierDefs[TierIndex];

        FMortisRuneSynergyEntryViewData EntryData;
        EntryData.DisplayIcon = SetRow->Icon.IsValidIndex(TierIndex) ? SetRow->Icon[TierIndex] : nullptr;
        EntryData.RequiredCount = TierDef.ActivateCount;
        EntryData.TitleText = FText::Format(
            NSLOCTEXT("MortisInventory", "RuneSynergyEntryTitleFormat", "{0}개 세트 효과"),
            FText::AsNumber(TierDef.ActivateCount));
        EntryData.DescriptionText = TierDef.Description;
        EntryData.bIsActive = CurrentEquippedCount >= TierDef.ActivateCount;

        OutEntries.Add(EntryData);
    }
}

int32 UMortisInventoryRunePageWidget::FindFirstEmptyRuneSlot() const
{
    if (!RuneInventorySubsystemRef)
    {
        return INDEX_NONE;
    }

    const TArray<FMortisRuneInstance>& EquippedRunes = RuneInventorySubsystemRef->GetEquippedRunes();
    for (int32 Index = 0; Index < EquippedRunes.Num(); ++Index)
    {
        if (!EquippedRunes[Index].InstanceId.IsValid())
        {
            return Index;
        }
    }

    return INDEX_NONE;
}

bool UMortisInventoryRunePageWidget::CanEquipSelectedRune() const
{
    if (!HasSelectedRune())
    {
        return false;
    }

    if (IsRuneEquipped(SelectedRune.InstanceId))
    {
        return false;
    }

    return FindFirstEmptyRuneSlot() != INDEX_NONE;
}

FText UMortisInventoryRunePageWidget::GetEquipButtonLabelText() const
{
    if (!HasSelectedRune())
    {
        return NSLOCTEXT("MortisInventory", "EquipRuneLabelDefault", "Equip");
    }

    if (IsRuneEquipped(SelectedRune.InstanceId))
    {
        return NSLOCTEXT("MortisInventory", "EquipRuneLabelEquipped", "Equipped");
    }

    if (FindFirstEmptyRuneSlot() == INDEX_NONE)
    {
        return NSLOCTEXT("MortisInventory", "EquipRuneLabelNoSlot", "No Empty Slot");
    }

    return NSLOCTEXT("MortisInventory", "EquipRuneLabelReady", "Equip");
}
