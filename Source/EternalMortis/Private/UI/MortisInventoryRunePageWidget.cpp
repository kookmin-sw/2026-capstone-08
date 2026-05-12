#include "UI/MortisInventoryRunePageWidget.h"

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
#include "UI/MortisEquippedRunePanelWidget.h"
#include "UI/MortisRuneCardWidget.h"
#include "UI/MortisRunePresentationStyle.h"
#include "UI/MortisRightRuneDetailWidget.h"
#include "UI/MortisRuneSetEntryWidget.h"

void UMortisInventoryRunePageWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeReferences();
    BindInventoryDelegates();
    BindRightRuneDetailEvents();
    InitializeEquippedRunePanel();
    BindEquippedRunePanelEvents();

    RefreshAll();
}

void UMortisInventoryRunePageWidget::NativeDestruct()
{
    UnbindEquippedRunePanelEvents();
    UnbindRightRuneDetailEvents();
    UnbindInventoryDelegates();

    Super::NativeDestruct();
}

void UMortisInventoryRunePageWidget::RefreshAll()
{
    RefreshSetList();
    UpdateCenterState();
    RefreshEquippedRunePanel();
    RefreshRuneGrid();
    RefreshRightPanel();
}

void UMortisInventoryRunePageWidget::HandleSetEntryClicked(FGameplayTag ClickedSetTag)
{
    bIsAllRunesFilterSelected = !ClickedSetTag.IsValid();
    SelectedSetTag = ClickedSetTag;
    SelectedRune = FMortisRuneInstance();
    SelectedRuneSource = EMortisRuneSelectionSource::None;

    RefreshAll();
}

void UMortisInventoryRunePageWidget::HandleRuneCardClicked(FMortisRuneInstance ClickedRune)
{
    SelectedRune = ClickedRune;
    SelectedRuneSource = EMortisRuneSelectionSource::Inventory;

    RefreshRuneGrid();
    RefreshEquippedRunePanel();
    RefreshRightPanel();
}

void UMortisInventoryRunePageWidget::HandleEquippedRuneCardClicked(FMortisRuneInstance ClickedRune)
{
    if (!ClickedRune.InstanceId.IsValid())
    {
        return;
    }

    SelectedRune = ClickedRune;
    SelectedRuneSource = EMortisRuneSelectionSource::Equipped;

    RefreshRuneGrid();
    RefreshEquippedRunePanel();
    RefreshRightPanel();
}

void UMortisInventoryRunePageWidget::HandleEquipButtonClicked()
{
    if (!RuneInventorySubsystemRef || !HasSelectedRune() || SelectedRuneSource != EMortisRuneSelectionSource::Inventory)
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

    SelectedRuneSource = EMortisRuneSelectionSource::Equipped;

    if (!RuneInventorySubsystemRef->EquipRune(TargetSlotIndex, SelectedRune))
    {
        SelectedRuneSource = EMortisRuneSelectionSource::Inventory;
    }
}

void UMortisInventoryRunePageWidget::HandleOwningRuneAdded(const FMortisRuneInstance& AddedRune)
{
    RefreshAll();
}

void UMortisInventoryRunePageWidget::HandleOwningRuneRemoved(const FMortisRuneInstance& RemovedRune)
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

    RuneInventorySubsystemRef->OnOwningRuneAdded.AddDynamic(this, &ThisClass::HandleOwningRuneAdded);
    RuneInventorySubsystemRef->OnOwningRuneRemoved.AddDynamic(this, &ThisClass::HandleOwningRuneRemoved);
}

void UMortisInventoryRunePageWidget::UnbindInventoryDelegates()
{
    if (!RuneInventorySubsystemRef)
    {
        return;
    }

    RuneInventorySubsystemRef->OnOwningRuneAdded.RemoveDynamic(this, &ThisClass::HandleOwningRuneAdded);
    RuneInventorySubsystemRef->OnOwningRuneRemoved.RemoveDynamic(this, &ThisClass::HandleOwningRuneRemoved);
}

void UMortisInventoryRunePageWidget::BindRightRuneDetailEvents()
{
    if (UMortisRightRuneDetailWidget* RightRuneDetail = ResolveRightRuneDetailWidget())
    {
        RightRuneDetail->OnEquipRequested.RemoveDynamic(this, &ThisClass::HandleEquipButtonClicked);
        RightRuneDetail->OnEquipRequested.AddDynamic(this, &ThisClass::HandleEquipButtonClicked);
    }
}

void UMortisInventoryRunePageWidget::UnbindRightRuneDetailEvents()
{
    if (UMortisRightRuneDetailWidget* RightRuneDetail = ResolveRightRuneDetailWidget())
    {
        RightRuneDetail->OnEquipRequested.RemoveDynamic(this, &ThisClass::HandleEquipButtonClicked);
    }
}

void UMortisInventoryRunePageWidget::InitializeEquippedRunePanel()
{
    if (Widget_EquippedRunePanel)
    {
        Widget_EquippedRunePanel->InitializeEquippedRunePanel(RuneInventorySubsystemRef, RuneDatabaseSubsystemRef, RuneCardClass);
    }
}

void UMortisInventoryRunePageWidget::BindEquippedRunePanelEvents()
{
    if (Widget_EquippedRunePanel)
    {
        Widget_EquippedRunePanel->OnEquippedRuneSelected.RemoveDynamic(this, &ThisClass::HandleEquippedRuneCardClicked);
        Widget_EquippedRunePanel->OnEquippedRuneSelected.AddDynamic(this, &ThisClass::HandleEquippedRuneCardClicked);
    }
}

void UMortisInventoryRunePageWidget::UnbindEquippedRunePanelEvents()
{
    if (Widget_EquippedRunePanel)
    {
        Widget_EquippedRunePanel->OnEquippedRuneSelected.RemoveDynamic(this, &ThisClass::HandleEquippedRuneCardClicked);
    }
}

void UMortisInventoryRunePageWidget::RefreshSetList()
{
    if (!ScrollBox_SetList)
    {
        return;
    }

    ScrollBox_SetList->ClearChildren();
    CreatedSetEntries.Reset();

    if (!RuneSetEntryClass)
    {
        return;
    }

    UMortisRuneSetEntryWidget* AllEntry = CreateWidget<UMortisRuneSetEntryWidget>(this, RuneSetEntryClass);
    if (AllEntry)
    {
        AllEntry->ApplyData(
            FGameplayTag(),
            nullptr,
            NSLOCTEXT("MortisInventory", "AllRunesFilterName", "전체"),
            GetInventoryRuneCount(),
            bIsAllRunesFilterSelected);
        AllEntry->OnSetEntryClicked.RemoveDynamic(this, &ThisClass::HandleSetEntryClicked);
        AllEntry->OnSetEntryClicked.AddDynamic(this, &ThisClass::HandleSetEntryClicked);

        ScrollBox_SetList->AddChild(AllEntry);
        CreatedSetEntries.Add(AllEntry);
    }

    if (!RuneSetTableRef)
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
        const bool bIsSelected = !bIsAllRunesFilterSelected && SelectedSetTag.IsValid() && SelectedSetTag == SetRow->SetTag;

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
        const int32 TargetIndex = WidgetSwitcher_CenterState->GetChildrenCount() > CenterInventoryStateIndex
            ? CenterInventoryStateIndex
            : 0;
        WidgetSwitcher_CenterState->SetActiveWidgetIndex(TargetIndex);
    }

    if (Text_EmptyMessage)
    {
        Text_EmptyMessage->SetText(NSLOCTEXT("MortisInventory", "NoOwnedRunesMessage", "No owned runes."));
    }

    UpdateCenterHeader();
}

void UMortisInventoryRunePageWidget::UpdateCenterHeader()
{
    if (Text_SelectedSetName)
    {
        if (bIsAllRunesFilterSelected)
        {
            Text_SelectedSetName->SetText(NSLOCTEXT("MortisInventory", "AllRunesHeader", "전체 룬"));
        }
        else if (RuneDatabaseSubsystemRef)
        {
            const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef->GetRuneSetRow(SelectedSetTag);
            Text_SelectedSetName->SetText(SetRow ? SetRow->SetName : FText::GetEmpty());
        }
    }

    if (Text_RuneCount)
    {
        if (bIsAllRunesFilterSelected)
        {
            Text_RuneCount->SetText(FText::Format(
                NSLOCTEXT("MortisInventory", "AllRuneCountFormat", "{0} Runes"),
                FText::AsNumber(GetInventoryRuneCount())));
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

void UMortisInventoryRunePageWidget::RefreshEquippedRunePanel()
{
    ValidateSelectedEquippedRune();

    if (!Widget_EquippedRunePanel)
    {
        return;
    }

    Widget_EquippedRunePanel->SetSelectedRuneState(SelectedRune, SelectedRuneSource == EMortisRuneSelectionSource::Equipped);
}

void UMortisInventoryRunePageWidget::RefreshRuneGrid()
{
    ClearRuneGrid();

    UpdateCenterHeader();

    if (!UniformGrid_RuneGrid || !RuneCardClass || !RuneInventorySubsystemRef)
    {
        return;
    }

    TArray<FMortisRuneInstance> MatchingRunes;
    GetOwningRunesForCurrentFilter(MatchingRunes);

    if (HasSelectedRune() && SelectedRuneSource == EMortisRuneSelectionSource::Inventory)
    {
        const bool bStillExists = MatchingRunes.ContainsByPredicate(
            [this](const FMortisRuneInstance& Rune)
            {
                return Rune.InstanceId == SelectedRune.InstanceId;
            });

        if (!bStillExists)
        {
            SelectedRune = FMortisRuneInstance();
            SelectedRuneSource = EMortisRuneSelectionSource::None;
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

        RuneCard->SetCardSize(InventoryRuneCardSize);

        const bool bIsSelected = HasSelectedRune() && SelectedRuneSource == EMortisRuneSelectionSource::Inventory && SelectedRune.InstanceId == Rune.InstanceId;
        const bool bIsEquipped = IsRuneEquipped(Rune.InstanceId);

        ConfigureRuneCard(RuneCard, Rune, bIsSelected, bIsEquipped);

        const int32 Row = Index / SafeColumns;
        const int32 Column = Index % SafeColumns;

        if (UUniformGridSlot* GridSlot = UniformGrid_RuneGrid->AddChildToUniformGrid(RuneCard, Row, Column))
        {
            GridSlot->SetHorizontalAlignment(HAlign_Center);
            GridSlot->SetVerticalAlignment(VAlign_Center);
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
        const int32 FallbackRuneDetailIndex = WidgetSwitcher_RightState->GetChildrenCount() > 1 ? 1 : 0;
        const int32 TargetIndex = WidgetSwitcher_RightState->GetChildrenCount() > RightRuneDetailStateIndex
            ? RightRuneDetailStateIndex
            : FallbackRuneDetailIndex;
        WidgetSwitcher_RightState->SetActiveWidgetIndex(TargetIndex);
        UpdateRightRuneDetail();
        return;
    }

    const int32 TargetIndex = WidgetSwitcher_RightState->GetChildrenCount() > RightSetDetailStateIndex
        ? RightSetDetailStateIndex
        : 0;
    WidgetSwitcher_RightState->SetActiveWidgetIndex(TargetIndex);
    UpdateRightSetDetail();
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

UMortisRightRuneDetailWidget* UMortisInventoryRunePageWidget::ResolveRightRuneDetailWidget()
{
    if (Widget_RightRuneDetail)
    {
        return Widget_RightRuneDetail;
    }

    if (!WidgetSwitcher_RightState)
    {
        return nullptr;
    }

    const int32 ChildCount = WidgetSwitcher_RightState->GetChildrenCount();
    for (int32 ChildIndex = 0; ChildIndex < ChildCount; ++ChildIndex)
    {
        if (UMortisRightRuneDetailWidget* RightRuneDetail = Cast<UMortisRightRuneDetailWidget>(WidgetSwitcher_RightState->GetChildAt(ChildIndex)))
        {
            Widget_RightRuneDetail = RightRuneDetail;
            return Widget_RightRuneDetail;
        }
    }

    return nullptr;
}

void UMortisInventoryRunePageWidget::ValidateSelectedEquippedRune()
{
    if (SelectedRuneSource != EMortisRuneSelectionSource::Equipped || !SelectedRune.InstanceId.IsValid())
    {
        return;
    }

    if (!RuneInventorySubsystemRef)
    {
        SelectedRune = FMortisRuneInstance();
        SelectedRuneSource = EMortisRuneSelectionSource::None;
        return;
    }

    const bool bSelectedRuneStillExists = RuneInventorySubsystemRef->GetEquippedRunes().ContainsByPredicate(
        [this](const FMortisRuneInstance& Rune)
        {
            return Rune.InstanceId.IsValid() && Rune.InstanceId == SelectedRune.InstanceId;
        });

    if (!bSelectedRuneStillExists)
    {
        SelectedRune = FMortisRuneInstance();
        SelectedRuneSource = EMortisRuneSelectionSource::None;
    }
}

void UMortisInventoryRunePageWidget::GetOwningRunesForCurrentFilter(TArray<FMortisRuneInstance>& OutRunes) const
{
    OutRunes.Reset();

    if (!RuneInventorySubsystemRef)
    {
        return;
    }

    if (bIsAllRunesFilterSelected || !SelectedSetTag.IsValid())
    {
        OutRunes = RuneInventorySubsystemRef->GetOwningRunes();
        return;
    }

    GetOwningRunesBySetTag(SelectedSetTag, OutRunes);
}

int32 UMortisInventoryRunePageWidget::GetInventoryRuneCount() const
{
    return RuneInventorySubsystemRef ? RuneInventorySubsystemRef->GetOwningRunes().Num() : 0;
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

void UMortisInventoryRunePageWidget::UpdateRightSetDetail()
{
    if (Text_RightSetTitle)
    {
        Text_RightSetTitle->SetText(bIsAllRunesFilterSelected
            ? NSLOCTEXT("MortisInventory", "RightAllRunesTitle", "전체 룬")
            : NSLOCTEXT("MortisInventory", "RightSetTitle", "Set Details"));
    }

    const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef
        ? RuneDatabaseSubsystemRef->GetRuneSetRow(SelectedSetTag)
        : nullptr;

    if (Text_SelectedSetName_Right)
    {
        Text_SelectedSetName_Right->SetText(bIsAllRunesFilterSelected
            ? NSLOCTEXT("MortisInventory", "AllRunesFilterName", "전체")
            : (SetRow ? SetRow->SetName : FText::GetEmpty()));
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

    if (!bIsAllRunesFilterSelected && SetRow)
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
    UMortisRightRuneDetailWidget* RightRuneDetail = ResolveRightRuneDetailWidget();
    if (!RightRuneDetail || !HasSelectedRune())
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

    ViewData.bHasRuneData = true;
    ViewData.RuneIcon = SymbolRow ? SymbolRow->Glyph : nullptr;
    ViewData.RuneIconTint = GetGlyphTintBySetTag(DetailSetTag);
    ViewData.RunePresentationStyle = MortisRunePresentation::BuildStyle(SelectedRune.Grade, ViewData.RuneIconTint);
    ViewData.RuneName = GetSelectedRuneNameText();
    ViewData.RuneStatValue = BuildRuneStatValueText();
    ViewData.SetName = SetRow ? SetRow->SetName : FText::GetEmpty();
    ViewData.SetProgress = BuildSetProgressTextBySetTag(DetailSetTag);
    ViewData.bShowEquipAction = SelectedRuneSource == EMortisRuneSelectionSource::Inventory && !IsRuneEquipped(SelectedRune.InstanceId);
    ViewData.bCanEquip = CanEquipSelectedRune();
    ViewData.EquipLabel = GetEquipButtonLabelText();
    BuildSelectedRuneSynergyEntries(DetailSetTag, ViewData.SynergyEntries);

    RightRuneDetail->ApplyData(ViewData);
}

FText UMortisInventoryRunePageWidget::BuildSelectedSetCountText() const
{
    if (bIsAllRunesFilterSelected)
    {
        return FText::Format(
            NSLOCTEXT("MortisInventory", "AllRunesInventoryCountFormat", "{0} inventory runes"),
            FText::AsNumber(GetInventoryRuneCount()));
    }

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
    const int32 MaxSearchCount = FMath::Min(EquippedRunes.Num(), FMath::Max(0, RuneInventorySubsystemRef->GetSlotCount()));
    for (int32 Index = 0; Index < MaxSearchCount; ++Index)
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
    if (!HasSelectedRune() || SelectedRuneSource != EMortisRuneSelectionSource::Inventory)
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
