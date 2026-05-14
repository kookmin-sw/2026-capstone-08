#include "UI/MortisRuneUnequipWidget.h"

#include "MortisFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "System/MortisRuneDatabaseSubsystem.h"
#include "System/MortisRuneInventorySubsystem.h"
#include "System/MortisRunStateSubsystem.h"
#include "UI/MortisEquippedRunePanelWidget.h"
#include "UI/MortisRightRuneDetailWidget.h"
#include "UI/MortisRuneCardWidget.h"
#include "UI/MortisRunePresentationStyle.h"

void UMortisRuneUnequipWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetIsFocusable(true);

    InitializeReferences();

    if (Widget_EquippedRunePanel)
    {
        Widget_EquippedRunePanel->InitializeEquippedRunePanel(RuneInventorySubsystemRef, RuneDatabaseSubsystemRef, RuneCardClass);
    }

    BindDelegates();
    ResetScreenState();
    SetKeyboardFocus();
}

void UMortisRuneUnequipWidget::NativeDestruct()
{
    UnbindDelegates();

    Super::NativeDestruct();
}

FReply UMortisRuneUnequipWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::Escape)
    {
        CloseScreen();
        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UMortisRuneUnequipWidget::RefreshAll()
{
    if (HasSelectedRune() && !IsSelectedRuneStillEquipped())
    {
        ClearSelectedRune();
        SetConfirmOverlayVisible(false);
    }

    RefreshEquippedRunePanel();
    RefreshRightRuneDetail();
    RefreshConfirmationText();
}

void UMortisRuneUnequipWidget::CloseScreen()
{
    ResetScreenState();

    if (APlayerController* PlayerController = GetOwningPlayer())
    {
        FInputModeGameOnly InputMode;
        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = false;
    }

    RemoveFromParent();
}

void UMortisRuneUnequipWidget::ResetScreenState()
{
    ClearSelectedRune();
    SetConfirmOverlayVisible(false);

    if (Widget_EquippedRunePanel)
    {
        Widget_EquippedRunePanel->ClearSelectedRuneState();
        Widget_EquippedRunePanel->RefreshEquippedRunes();
    }

    if (Widget_RightRuneDetail)
    {
        Widget_RightRuneDetail->ApplyData(FMortisRightRuneDetailViewData());
    }

    RefreshConfirmationText();
}

void UMortisRuneUnequipWidget::HandleEquippedRuneSelected(FMortisRuneInstance ClickedRune)
{
    if (!ClickedRune.InstanceId.IsValid())
    {
        return;
    }

    SelectedRune = ClickedRune;
    SetConfirmOverlayVisible(false);
    RefreshAll();
}

void UMortisRuneUnequipWidget::HandleUnequipRequested()
{
    if (!CanUnequipSelectedRune())
    {
        SetConfirmOverlayVisible(false);
        RefreshAll();
        return;
    }

    RefreshConfirmationText();
    SetConfirmOverlayVisible(true);
}

void UMortisRuneUnequipWidget::HandleConfirmUnequipClicked()
{
    const int32 SlotIndex = FindSelectedRuneSlotIndex();
    if (SlotIndex == INDEX_NONE || !RunStateSubsystemRef || !RuneInventorySubsystemRef || !RunStateSubsystemRef->HasEnoughGold(UnequipCost))
    {
        SetConfirmOverlayVisible(false);
        RefreshAll();
        return;
    }

    if (!RunStateSubsystemRef->SpendGold(UnequipCost))
    {
        SetConfirmOverlayVisible(false);
        RefreshAll();
        return;
    }

    if (!RuneInventorySubsystemRef->UnequipRune(SlotIndex))
    {
        RunStateSubsystemRef->AddGold(UnequipCost);
        SetConfirmOverlayVisible(false);
        RefreshAll();
        return;
    }

    ClearSelectedRune();
    SetConfirmOverlayVisible(false);
    RefreshAll();
}

void UMortisRuneUnequipWidget::HandleCancelUnequipClicked()
{
    SetConfirmOverlayVisible(false);
    RefreshAll();
}

void UMortisRuneUnequipWidget::HandleCloseClicked()
{
    CloseScreen();
}

void UMortisRuneUnequipWidget::HandleGoldChanged(int32)
{
    RefreshRightRuneDetail();
    RefreshConfirmationText();
}

void UMortisRuneUnequipWidget::HandleEquippedRuneAdded(int32, const FMortisRuneInstance&)
{
    RefreshAll();
}

void UMortisRuneUnequipWidget::HandleEquippedRuneRemoved(int32, const FMortisRuneInstance& RemovedRune)
{
    if (SelectedRune.InstanceId.IsValid() && RemovedRune.InstanceId == SelectedRune.InstanceId)
    {
        ClearSelectedRune();
        SetConfirmOverlayVisible(false);
    }

    RefreshAll();
}

void UMortisRuneUnequipWidget::InitializeReferences()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        RuneInventorySubsystemRef = GameInstance->GetSubsystem<UMortisRuneInventorySubsystem>();
        RuneDatabaseSubsystemRef = GameInstance->GetSubsystem<UMortisRuneDatabaseSubsystem>();
        RunStateSubsystemRef = GameInstance->GetSubsystem<UMortisRunStateSubsystem>();
    }

    if (!RuneCardClass)
    {
        static const FSoftClassPath DefaultRuneCardClassPath(TEXT("/Game/UI/Inventory/WBP/Rune_Inventory/WBP_RuneCard.WBP_RuneCard_C"));
        RuneCardClass = Cast<UClass>(DefaultRuneCardClassPath.TryLoad());
    }
}

void UMortisRuneUnequipWidget::BindDelegates()
{
    if (Widget_EquippedRunePanel)
    {
        Widget_EquippedRunePanel->OnEquippedRuneSelected.RemoveDynamic(this, &ThisClass::HandleEquippedRuneSelected);
        Widget_EquippedRunePanel->OnEquippedRuneSelected.AddDynamic(this, &ThisClass::HandleEquippedRuneSelected);
    }

    if (Widget_RightRuneDetail)
    {
        Widget_RightRuneDetail->OnUnequipRequested.RemoveDynamic(this, &ThisClass::HandleUnequipRequested);
        Widget_RightRuneDetail->OnUnequipRequested.AddDynamic(this, &ThisClass::HandleUnequipRequested);
    }

    if (RunStateSubsystemRef)
    {
        RunStateSubsystemRef->OnGoldChanged.RemoveDynamic(this, &ThisClass::HandleGoldChanged);
        RunStateSubsystemRef->OnGoldChanged.AddDynamic(this, &ThisClass::HandleGoldChanged);
    }

    if (RuneInventorySubsystemRef)
    {
        RuneInventorySubsystemRef->OnEquippedRuneAdded.RemoveDynamic(this, &ThisClass::HandleEquippedRuneAdded);
        RuneInventorySubsystemRef->OnEquippedRuneRemoved.RemoveDynamic(this, &ThisClass::HandleEquippedRuneRemoved);
        RuneInventorySubsystemRef->OnEquippedRuneAdded.AddDynamic(this, &ThisClass::HandleEquippedRuneAdded);
        RuneInventorySubsystemRef->OnEquippedRuneRemoved.AddDynamic(this, &ThisClass::HandleEquippedRuneRemoved);
    }

    if (Button_ConfirmUnequip)
    {
        Button_ConfirmUnequip->OnClicked.RemoveDynamic(this, &ThisClass::HandleConfirmUnequipClicked);
        Button_ConfirmUnequip->OnClicked.AddDynamic(this, &ThisClass::HandleConfirmUnequipClicked);
    }

    if (Button_CancelUnequip)
    {
        Button_CancelUnequip->OnClicked.RemoveDynamic(this, &ThisClass::HandleCancelUnequipClicked);
        Button_CancelUnequip->OnClicked.AddDynamic(this, &ThisClass::HandleCancelUnequipClicked);
    }

    if (Button_Close)
    {
        Button_Close->OnClicked.RemoveDynamic(this, &ThisClass::HandleCloseClicked);
        Button_Close->OnClicked.AddDynamic(this, &ThisClass::HandleCloseClicked);
    }
}

void UMortisRuneUnequipWidget::UnbindDelegates()
{
    if (Widget_EquippedRunePanel)
    {
        Widget_EquippedRunePanel->OnEquippedRuneSelected.RemoveDynamic(this, &ThisClass::HandleEquippedRuneSelected);
    }

    if (Widget_RightRuneDetail)
    {
        Widget_RightRuneDetail->OnUnequipRequested.RemoveDynamic(this, &ThisClass::HandleUnequipRequested);
    }

    if (RunStateSubsystemRef)
    {
        RunStateSubsystemRef->OnGoldChanged.RemoveDynamic(this, &ThisClass::HandleGoldChanged);
    }

    if (RuneInventorySubsystemRef)
    {
        RuneInventorySubsystemRef->OnEquippedRuneAdded.RemoveDynamic(this, &ThisClass::HandleEquippedRuneAdded);
        RuneInventorySubsystemRef->OnEquippedRuneRemoved.RemoveDynamic(this, &ThisClass::HandleEquippedRuneRemoved);
    }

    if (Button_ConfirmUnequip)
    {
        Button_ConfirmUnequip->OnClicked.RemoveDynamic(this, &ThisClass::HandleConfirmUnequipClicked);
    }

    if (Button_CancelUnequip)
    {
        Button_CancelUnequip->OnClicked.RemoveDynamic(this, &ThisClass::HandleCancelUnequipClicked);
    }

    if (Button_Close)
    {
        Button_Close->OnClicked.RemoveDynamic(this, &ThisClass::HandleCloseClicked);
    }
}

void UMortisRuneUnequipWidget::RefreshEquippedRunePanel()
{
    if (!Widget_EquippedRunePanel)
    {
        return;
    }

    if (HasSelectedRune())
    {
        Widget_EquippedRunePanel->SetSelectedRuneState(SelectedRune, true);
    }
    else
    {
        Widget_EquippedRunePanel->ClearSelectedRuneState();
        Widget_EquippedRunePanel->RefreshEquippedRunes();
    }
}

void UMortisRuneUnequipWidget::RefreshRightRuneDetail()
{
    if (!Widget_RightRuneDetail)
    {
        return;
    }

    if (!HasSelectedRune() || !IsSelectedRuneStillEquipped())
    {
        Widget_RightRuneDetail->ApplyData(FMortisRightRuneDetailViewData());
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
    ViewData.bShowUnequipAction = true;
    ViewData.bCanUnequip = CanUnequipSelectedRune();
    ViewData.UnequipLabel = NSLOCTEXT("MortisRuneUnequip", "UnequipButtonLabel", "Remove");
    ViewData.UnequipCostText = BuildUnequipCostText();
    ViewData.CurrentGoldText = BuildCurrentGoldText();
    BuildSelectedRuneSynergyEntries(DetailSetTag, ViewData.SynergyEntries);

    Widget_RightRuneDetail->ApplyData(ViewData);
}

void UMortisRuneUnequipWidget::RefreshConfirmationText()
{
    if (Text_ConfirmUnequipMessage)
    {
        Text_ConfirmUnequipMessage->SetText(NSLOCTEXT(
            "MortisRuneUnequip",
            "ConfirmUnequipMessage",
            "정말 제거하시겠습니까?"));
    }

    if (Text_ConfirmUnequipCost)
    {
        Text_ConfirmUnequipCost->SetText(FText::Format(
            NSLOCTEXT("MortisRuneUnequip", "ConfirmUnequipCostFormat", "Gold 소모량: {0}"),
            FText::AsNumber(FMath::Max(0, UnequipCost))));
    }
}

void UMortisRuneUnequipWidget::ClearSelectedRune()
{
    SelectedRune = FMortisRuneInstance();
}

void UMortisRuneUnequipWidget::SetConfirmOverlayVisible(bool bVisible)
{
    if (Overlay_ConfirmUnequip)
    {
        Overlay_ConfirmUnequip->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

bool UMortisRuneUnequipWidget::HasSelectedRune() const
{
    return SelectedRune.InstanceId.IsValid();
}

bool UMortisRuneUnequipWidget::IsSelectedRuneStillEquipped() const
{
    return FindSelectedRuneSlotIndex() != INDEX_NONE;
}

bool UMortisRuneUnequipWidget::CanUnequipSelectedRune() const
{
    return FindSelectedRuneSlotIndex() != INDEX_NONE
        && RunStateSubsystemRef
        && RunStateSubsystemRef->HasEnoughGold(UnequipCost);
}

int32 UMortisRuneUnequipWidget::FindSelectedRuneSlotIndex() const
{
    if (!RuneInventorySubsystemRef || !SelectedRune.InstanceId.IsValid())
    {
        return INDEX_NONE;
    }

    const TArray<FMortisRuneInstance>& EquippedRunes = RuneInventorySubsystemRef->GetEquippedRunes();
    const int32 SearchCount = FMath::Min(EquippedRunes.Num(), FMath::Max(0, RuneInventorySubsystemRef->GetSlotCount()));

    for (int32 SlotIndex = 0; SlotIndex < SearchCount; ++SlotIndex)
    {
        const FMortisRuneInstance& EquippedRune = EquippedRunes[SlotIndex];
        if (EquippedRune.InstanceId.IsValid() && EquippedRune.InstanceId == SelectedRune.InstanceId)
        {
            return SlotIndex;
        }
    }

    return INDEX_NONE;
}

int32 UMortisRuneUnequipWidget::GetEquippedCountBySetTag(const FGameplayTag& SetTag) const
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

FText UMortisRuneUnequipWidget::GetSelectedRuneNameText() const
{
    if (!HasSelectedRune() || !RuneDatabaseSubsystemRef)
    {
        return FText::GetEmpty();
    }

    const FMortisRuneSymbolRow* SymbolRow = RuneDatabaseSubsystemRef->GetRuneSymbolRow(SelectedRune.SymbolType);
    if (SymbolRow)
    {
        return FText::Format(
            NSLOCTEXT("MortisRuneUnequip", "SelectedRuneNameFormat", "{0} Rune"),
            SymbolRow->DisplayName);
    }

    const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef->GetRuneSetRow(SelectedRune.SetTag);
    if (SetRow)
    {
        return SetRow->SetName;
    }

    return FText::FromString(SelectedRune.SetTag.ToString());
}

FText UMortisRuneUnequipWidget::BuildRuneStatValueText() const
{
    if (!HasSelectedRune())
    {
        return FText::GetEmpty();
    }

    const FMortisRuneSymbolRow* SymbolRow = RuneDatabaseSubsystemRef
        ? RuneDatabaseSubsystemRef->GetRuneSymbolRow(SelectedRune.SymbolType)
        : nullptr;

    return UMortisFunctionLibrary::FormatSignedRuneValue(
        SelectedRune.RolledValue,
        SymbolRow ? SymbolRow->ValueFractionalDigits : 0,
        SymbolRow ? SymbolRow->bDisplayAsPercent : false);
}

FText UMortisRuneUnequipWidget::BuildSetProgressTextBySetTag(const FGameplayTag& SetTag) const
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
                NSLOCTEXT("MortisRuneUnequip", "SetProgressNextTierFormat", "Equipped {0} / next effect at {1}"),
                FText::AsNumber(CurrentEquippedCount),
                FText::AsNumber(TierDef.ActivateCount));
        }
    }

    return FText::Format(
        NSLOCTEXT("MortisRuneUnequip", "SetProgressCompletedFormat", "Equipped {0} / all effects active"),
        FText::AsNumber(CurrentEquippedCount));
}

FText UMortisRuneUnequipWidget::BuildUnequipCostText() const
{
    return FText::Format(
        NSLOCTEXT("MortisRuneUnequip", "UnequipCostFormat", "Cost: {0} Gold"),
        FText::AsNumber(FMath::Max(0, UnequipCost)));
}

FText UMortisRuneUnequipWidget::BuildCurrentGoldText() const
{
    return FText::Format(
        NSLOCTEXT("MortisRuneUnequip", "CurrentGoldFormat", "Gold: {0}"),
        FText::AsNumber(RunStateSubsystemRef ? RunStateSubsystemRef->GetCurrentGold() : 0));
}

FLinearColor UMortisRuneUnequipWidget::GetGlyphTintBySetTag(const FGameplayTag& SetTag) const
{
    if (!RuneDatabaseSubsystemRef || !SetTag.IsValid())
    {
        return FLinearColor::White;
    }

    const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystemRef->GetRuneSetRow(SetTag);
    return SetRow ? SetRow->GlyphTint : FLinearColor::White;
}

void UMortisRuneUnequipWidget::BuildSelectedRuneSynergyEntries(const FGameplayTag& SetTag, TArray<FMortisRuneSynergyEntryViewData>& OutEntries) const
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
            NSLOCTEXT("MortisRuneUnequip", "RuneSynergyEntryTitleFormat", "{0}-piece effect"),
            FText::AsNumber(TierDef.ActivateCount));
        EntryData.DescriptionText = TierDef.Description;
        EntryData.bIsActive = CurrentEquippedCount >= TierDef.ActivateCount;

        OutEntries.Add(EntryData);
    }
}
