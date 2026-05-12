#include "UI/MortisEquippedRunePanelWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Engine/GameInstance.h"
#include "System/MortisRuneDatabaseSubsystem.h"
#include "System/MortisRuneInventorySubsystem.h"
#include "UI/MortisRuneCardWidget.h"

void UMortisEquippedRunePanelWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeReferences();
    BindInventoryDelegates();
    RefreshEquippedRunes();
}

void UMortisEquippedRunePanelWidget::NativeDestruct()
{
    UnbindInventoryDelegates();

    Super::NativeDestruct();
}

void UMortisEquippedRunePanelWidget::InitializeEquippedRunePanel(
    UMortisRuneInventorySubsystem* InRuneInventorySubsystem,
    UMortisRuneDatabaseSubsystem* InRuneDatabaseSubsystem,
    TSubclassOf<UMortisRuneCardWidget> InRuneCardClass)
{
    UnbindInventoryDelegates();

    if (InRuneInventorySubsystem)
    {
        RuneInventorySubsystemRef = InRuneInventorySubsystem;
    }

    if (InRuneDatabaseSubsystem)
    {
        RuneDatabaseSubsystemRef = InRuneDatabaseSubsystem;
    }

    if (InRuneCardClass)
    {
        RuneCardClass = InRuneCardClass;
    }

    InitializeReferences();
    BindInventoryDelegates();
    RefreshEquippedRunes();
}

void UMortisEquippedRunePanelWidget::RefreshEquippedRunes()
{
    ClearEquippedRuneGrid();

    if (!UniformGrid_EquippedRuneGrid || !RuneCardClass || !RuneInventorySubsystemRef)
    {
        return;
    }

    if (bSelectedFromEquipped && !IsSelectedEquippedRuneStillValid())
    {
        ClearSelectedRuneState();
    }

    const TArray<FMortisRuneInstance>& EquippedRunes = RuneInventorySubsystemRef->GetEquippedRunes();
    const int32 SafeColumns = FMath::Max(1, EquippedRuneColumns);
    const int32 DisplayedSlotCount = FMath::Max(0, MaxDisplayedEquippedRuneSlots);
    const int32 UnlockedSlotCount = FMath::Clamp(RuneInventorySubsystemRef->GetSlotCount(), 0, DisplayedSlotCount);

    for (int32 SlotIndex = 0; SlotIndex < DisplayedSlotCount; ++SlotIndex)
    {
        UMortisRuneCardWidget* RuneCard = CreateWidget<UMortisRuneCardWidget>(this, RuneCardClass);
        if (!RuneCard)
        {
            continue;
        }

        RuneCard->SetCardSize(EquippedRuneCardSize);

        const bool bUnlocked = SlotIndex < UnlockedSlotCount;
        const FMortisRuneInstance RuneInstance = EquippedRunes.IsValidIndex(SlotIndex)
            ? EquippedRunes[SlotIndex]
            : FMortisRuneInstance();

        ConfigureRuneSlotCard(RuneCard, RuneInstance, bUnlocked);

        const int32 Row = SlotIndex / SafeColumns;
        const int32 Column = SlotIndex % SafeColumns;

        if (UUniformGridSlot* GridSlot = UniformGrid_EquippedRuneGrid->AddChildToUniformGrid(RuneCard, Row, Column))
        {
            GridSlot->SetHorizontalAlignment(HAlign_Center);
            GridSlot->SetVerticalAlignment(VAlign_Center);
        }

        CreatedRuneCards.Add(RuneCard);
    }
}

void UMortisEquippedRunePanelWidget::SetSelectedRuneState(const FMortisRuneInstance& InSelectedRune, bool bInSelectedFromEquipped)
{
    SelectedRune = InSelectedRune;
    bSelectedFromEquipped = bInSelectedFromEquipped && SelectedRune.InstanceId.IsValid();

    RefreshEquippedRunes();
}

void UMortisEquippedRunePanelWidget::ClearSelectedRuneState()
{
    SelectedRune = FMortisRuneInstance();
    bSelectedFromEquipped = false;
}

void UMortisEquippedRunePanelWidget::SetRuneCardClass(TSubclassOf<UMortisRuneCardWidget> InRuneCardClass)
{
    RuneCardClass = InRuneCardClass;
    RefreshEquippedRunes();
}

void UMortisEquippedRunePanelWidget::HandleEquippedRuneCardClicked(FMortisRuneInstance ClickedRune)
{
    if (!ClickedRune.InstanceId.IsValid())
    {
        return;
    }

    OnEquippedRuneSelected.Broadcast(ClickedRune);
}

void UMortisEquippedRunePanelWidget::HandleEquippedRuneAdded(int32, const FMortisRuneInstance&)
{
    RefreshEquippedRunes();
}

void UMortisEquippedRunePanelWidget::HandleEquippedRuneRemoved(int32, const FMortisRuneInstance&)
{
    RefreshEquippedRunes();
}

void UMortisEquippedRunePanelWidget::InitializeReferences()
{
    if ((!RuneInventorySubsystemRef || !RuneDatabaseSubsystemRef) && GetGameInstance())
    {
        UGameInstance* GameInstance = GetGameInstance();

        if (!RuneInventorySubsystemRef)
        {
            RuneInventorySubsystemRef = GameInstance->GetSubsystem<UMortisRuneInventorySubsystem>();
        }

        if (!RuneDatabaseSubsystemRef)
        {
            RuneDatabaseSubsystemRef = GameInstance->GetSubsystem<UMortisRuneDatabaseSubsystem>();
        }
    }
}

void UMortisEquippedRunePanelWidget::BindInventoryDelegates()
{
    if (!RuneInventorySubsystemRef)
    {
        return;
    }

    RuneInventorySubsystemRef->OnEquippedRuneAdded.RemoveDynamic(this, &ThisClass::HandleEquippedRuneAdded);
    RuneInventorySubsystemRef->OnEquippedRuneRemoved.RemoveDynamic(this, &ThisClass::HandleEquippedRuneRemoved);

    RuneInventorySubsystemRef->OnEquippedRuneAdded.AddDynamic(this, &ThisClass::HandleEquippedRuneAdded);
    RuneInventorySubsystemRef->OnEquippedRuneRemoved.AddDynamic(this, &ThisClass::HandleEquippedRuneRemoved);
}

void UMortisEquippedRunePanelWidget::UnbindInventoryDelegates()
{
    if (!RuneInventorySubsystemRef)
    {
        return;
    }

    RuneInventorySubsystemRef->OnEquippedRuneAdded.RemoveDynamic(this, &ThisClass::HandleEquippedRuneAdded);
    RuneInventorySubsystemRef->OnEquippedRuneRemoved.RemoveDynamic(this, &ThisClass::HandleEquippedRuneRemoved);
}

void UMortisEquippedRunePanelWidget::ClearEquippedRuneGrid()
{
    CreatedRuneCards.Reset();

    if (UniformGrid_EquippedRuneGrid)
    {
        UniformGrid_EquippedRuneGrid->ClearChildren();
    }
}

void UMortisEquippedRunePanelWidget::ConfigureRuneSlotCard(UMortisRuneCardWidget* RuneCard, const FMortisRuneInstance& RuneInstance, bool bUnlocked)
{
    if (!RuneCard)
    {
        return;
    }

    if (!bUnlocked)
    {
        RuneCard->ClearRuneData();
        RuneCard->SetSlotState(EMortisRuneCardSlotState::Locked);
        RuneCard->SetSelected(false);
        RuneCard->SetEquipped(false);
        RuneCard->RefreshVisualState();
        RuneCard->OnRuneCardClicked.RemoveDynamic(this, &ThisClass::HandleEquippedRuneCardClicked);
        RuneCard->SetIsEnabled(false);
        return;
    }

    if (!RuneInstance.InstanceId.IsValid())
    {
        RuneCard->ClearRuneData();
        RuneCard->SetSlotState(EMortisRuneCardSlotState::EmptyUnlocked);
        RuneCard->SetSelected(false);
        RuneCard->SetEquipped(false);
        RuneCard->RefreshVisualState();
        RuneCard->OnRuneCardClicked.RemoveDynamic(this, &ThisClass::HandleEquippedRuneCardClicked);
        RuneCard->SetIsEnabled(false);
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

    const bool bIsSelected = bSelectedFromEquipped
        && SelectedRune.InstanceId.IsValid()
        && SelectedRune.InstanceId == RuneInstance.InstanceId;

    RuneCard->ClearRuneData();
    RuneCard->SetSlotState(EMortisRuneCardSlotState::Rune);
    RuneCard->SetRuneInstance(RuneInstance);
    RuneCard->SetDisplayIcon(DisplayIcon);
    RuneCard->SetDisplayIconTint(DisplayIconTint);
    RuneCard->SetSelected(bIsSelected);
    RuneCard->SetEquipped(true);
    RuneCard->RefreshVisualState();
    RuneCard->OnRuneCardClicked.RemoveDynamic(this, &ThisClass::HandleEquippedRuneCardClicked);
    RuneCard->OnRuneCardClicked.AddDynamic(this, &ThisClass::HandleEquippedRuneCardClicked);
    RuneCard->SetIsEnabled(true);
}

FLinearColor UMortisEquippedRunePanelWidget::GetGlyphTintBySetTag(const FGameplayTag& SetTag) const
{
    if (!RuneDatabaseSubsystemRef || !SetTag.IsValid())
    {
        return FLinearColor::White;
    }

    const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef->GetRuneSetRow(SetTag);
    return SetRow ? SetRow->GlyphTint : FLinearColor::White;
}

bool UMortisEquippedRunePanelWidget::IsSelectedEquippedRuneStillValid() const
{
    if (!RuneInventorySubsystemRef || !SelectedRune.InstanceId.IsValid())
    {
        return false;
    }

    for (const FMortisRuneInstance& Rune : RuneInventorySubsystemRef->GetEquippedRunes())
    {
        if (Rune.InstanceId.IsValid() && Rune.InstanceId == SelectedRune.InstanceId)
        {
            return true;
        }
    }

    return false;
}
