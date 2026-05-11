#include "UI/MortisGoldChangeToastWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateColor.h"
#include "System/MortisMetaProgressionSubsystem.h"
#include "System/MortisRunStateSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogMortisCurrencyToast, Log, All);

namespace
{
    constexpr TCHAR MemoryFragmentsNamePrefix[] = TEXT("MemoryFragments");
    constexpr TCHAR GoldNamePrefix[] = TEXT("Gold");

    FName MakeCurrencyWidgetName(const TCHAR* Prefix, const TCHAR* Suffix)
    {
        return FName(*FString::Printf(TEXT("%s_%s"), Suffix, Prefix));
    }
}

bool UMortisGoldChangeToastWidget::Initialize()
{
    const bool bSuperInitialized = Super::Initialize();

    BuildFallbackWidgetTreeIfNeeded();

    return bSuperInitialized;
}

void UMortisGoldChangeToastWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeFromRunState();

    if (!IsDesignTime())
    {
        UE_LOG(LogMortisCurrencyToast, Log, TEXT("Constructed %s"), *GetName());
    }
}

void UMortisGoldChangeToastWidget::NativeDestruct()
{
    UnbindSubsystemDelegates();
    RunStateSubsystemRef = nullptr;
    MetaProgressionSubsystemRef = nullptr;

    Super::NativeDestruct();
}

void UMortisGoldChangeToastWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsCoalescing)
    {
        CoalesceElapsed += InDeltaTime;
        if (CoalesceElapsed >= CoalesceDuration)
        {
            FlushPendingCurrencyChanges();
        }
    }

    if (MemoryFragmentsEntry.bIsAnimating)
    {
        UpdateEntryAnimation(MemoryFragmentsEntry, InDeltaTime);
    }

    if (GoldEntry.bIsAnimating)
    {
        UpdateEntryAnimation(GoldEntry, InDeltaTime);
    }

    HidePanelIfIdle();
}

void UMortisGoldChangeToastWidget::InitializeFromRunState()
{
    UnbindSubsystemDelegates();

    RunStateSubsystemRef = nullptr;
    MetaProgressionSubsystemRef = nullptr;

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        RunStateSubsystemRef = GameInstance->GetSubsystem<UMortisRunStateSubsystem>();
        MetaProgressionSubsystemRef = GameInstance->GetSubsystem<UMortisMetaProgressionSubsystem>();
    }

    if (RunStateSubsystemRef)
    {
        const int32 CurrentGold = RunStateSubsystemRef->GetCurrentGold();
        GoldEntry.DisplayedTotal = CurrentGold;
        GoldEntry.StartTotal = CurrentGold;
        GoldEntry.TargetTotal = CurrentGold;
        PendingGoldTarget = CurrentGold;
    }

    if (MetaProgressionSubsystemRef)
    {
        LastKnownMemoryFragments = MetaProgressionSubsystemRef->GetMemoryFragments();
        MemoryFragmentsEntry.DisplayedTotal = LastKnownMemoryFragments;
        MemoryFragmentsEntry.StartTotal = LastKnownMemoryFragments;
        MemoryFragmentsEntry.TargetTotal = LastKnownMemoryFragments;
        PendingMemoryTarget = LastKnownMemoryFragments;
    }

    MemoryFragmentsEntry.PendingDelta = 0;
    MemoryFragmentsEntry.AnimationElapsed = 0.0f;
    MemoryFragmentsEntry.bIsAnimating = false;
    MemoryFragmentsEntry.bIsActive = false;

    GoldEntry.PendingDelta = 0;
    GoldEntry.AnimationElapsed = 0.0f;
    GoldEntry.bIsAnimating = false;
    GoldEntry.bIsActive = false;

    PendingGoldDelta = 0;
    PendingMemoryDelta = 0;
    bHasPendingGold = false;
    bHasPendingMemoryFragments = false;
    bIsCoalescing = false;
    CoalesceElapsed = 0.0f;

    BindSubsystemDelegates();
    RefreshVisual();
    UpdateEntryLayout();
    HidePanelIfIdle();

    if (!IsDesignTime())
    {
        UE_LOG(LogMortisCurrencyToast, Log, TEXT("Initialized. Gold=%d MemoryFragments=%d"),
            GoldEntry.DisplayedTotal,
            MemoryFragmentsEntry.DisplayedTotal);
    }
}

void UMortisGoldChangeToastWidget::RefreshVisual()
{
    if (!GoldIcon)
    {
        GoldIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Asset/Curse/T_MemoryFrag.T_MemoryFrag"));
    }

    if (!MemoryFragmentIcon)
    {
        MemoryFragmentIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Asset/Curse/T_MemoryFrag.T_MemoryFrag"));
    }

    RefreshEntryVisual(MemoryFragmentsEntry, MemoryFragmentIcon);
    RefreshEntryVisual(GoldEntry, GoldIcon);
}

void UMortisGoldChangeToastWidget::HandleGoldChanged(int32 DeltaGold)
{
    if (DeltaGold == 0)
    {
        return;
    }

    const int32 TargetGold = RunStateSubsystemRef ? RunStateSubsystemRef->GetCurrentGold() : FMath::Max(0, GoldEntry.TargetTotal + DeltaGold);
    QueueCurrencyChange(EMortisCurrencyToastKind::Gold, DeltaGold, TargetGold);
}

void UMortisGoldChangeToastWidget::HandleMemoryFragmentsChanged(int32 NewValue)
{
    const int32 DeltaMemoryFragments = NewValue - LastKnownMemoryFragments;
    LastKnownMemoryFragments = NewValue;

    if (DeltaMemoryFragments == 0)
    {
        return;
    }

    QueueCurrencyChange(EMortisCurrencyToastKind::MemoryFragments, DeltaMemoryFragments, NewValue);
}

void UMortisGoldChangeToastWidget::BuildFallbackWidgetTreeIfNeeded()
{
    if (!WidgetTree || WidgetTree->RootWidget)
    {
        return;
    }

    HorizontalBox_Root = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HorizontalBox_CurrencyToastRoot"));
    if (!HorizontalBox_Root)
    {
        return;
    }

    WidgetTree->RootWidget = HorizontalBox_Root;

    BuildCurrencyEntry(MemoryFragmentsEntry, EMortisCurrencyToastKind::MemoryFragments);
    BuildCurrencyEntry(GoldEntry, EMortisCurrencyToastKind::Gold);
    UpdateEntryLayout();
}

void UMortisGoldChangeToastWidget::BuildCurrencyEntry(FMortisCurrencyToastEntry& Entry, EMortisCurrencyToastKind Kind)
{
    if (!WidgetTree || !HorizontalBox_Root)
    {
        return;
    }

    const TCHAR* NamePrefix = Kind == EMortisCurrencyToastKind::MemoryFragments ? MemoryFragmentsNamePrefix : GoldNamePrefix;

    Entry.RootBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), MakeCurrencyWidgetName(NamePrefix, TEXT("SizeBox")));
    if (!Entry.RootBox)
    {
        return;
    }

    Entry.RootBox->SetWidthOverride(EntrySize.X);
    Entry.RootBox->SetHeightOverride(EntrySize.Y);
    Entry.RootBox->SetVisibility(ESlateVisibility::Collapsed);

    Entry.Slot = HorizontalBox_Root->AddChildToHorizontalBox(Entry.RootBox);
    Entry.Slot->SetHorizontalAlignment(HAlign_Right);
    Entry.Slot->SetVerticalAlignment(VAlign_Top);

    Entry.Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), MakeCurrencyWidgetName(NamePrefix, TEXT("Border")));
    Entry.Border->SetPadding(FMargin(10.0f, 6.0f));
    Entry.Border->SetBrushColor(FLinearColor(0.02f, 0.018f, 0.014f, 0.74f));
    Entry.RootBox->AddChild(Entry.Border);

    UOverlay* Overlay_Root = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), MakeCurrencyWidgetName(NamePrefix, TEXT("Overlay")));
    Entry.Border->SetContent(Overlay_Root);

    UHorizontalBox* HorizontalBox_Total = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), MakeCurrencyWidgetName(NamePrefix, TEXT("HorizontalBox")));
    UOverlaySlot* TotalSlot = Overlay_Root->AddChildToOverlay(HorizontalBox_Total);
    TotalSlot->SetHorizontalAlignment(HAlign_Right);
    TotalSlot->SetVerticalAlignment(VAlign_Center);

    Entry.IconImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), MakeCurrencyWidgetName(NamePrefix, TEXT("Image")));
    UHorizontalBoxSlot* IconSlot = HorizontalBox_Total->AddChildToHorizontalBox(Entry.IconImage);
    IconSlot->SetPadding(FMargin(0.0f, 0.0f, 7.0f, 0.0f));
    IconSlot->SetHorizontalAlignment(HAlign_Center);
    IconSlot->SetVerticalAlignment(VAlign_Center);
    Entry.IconImage->SetDesiredSizeOverride(FVector2D(32.0f, 32.0f));

    Entry.TotalText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), MakeCurrencyWidgetName(NamePrefix, TEXT("Text_Total")));
    UHorizontalBoxSlot* TotalTextSlot = HorizontalBox_Total->AddChildToHorizontalBox(Entry.TotalText);
    TotalTextSlot->SetHorizontalAlignment(HAlign_Right);
    TotalTextSlot->SetVerticalAlignment(VAlign_Center);

    FSlateFontInfo TotalFont = Entry.TotalText->GetFont();
    TotalFont.Size = 22;
    TotalFont.TypefaceFontName = TEXT("Bold");
    Entry.TotalText->SetFont(TotalFont);
    Entry.TotalText->SetColorAndOpacity(FSlateColor(TotalTextColor));
    Entry.TotalText->SetShadowOffset(FVector2D(1.0f, 1.0f));
    Entry.TotalText->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));

    Entry.DeltaText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), MakeCurrencyWidgetName(NamePrefix, TEXT("Text_Delta")));
    UOverlaySlot* DeltaSlot = Overlay_Root->AddChildToOverlay(Entry.DeltaText);
    DeltaSlot->SetHorizontalAlignment(HAlign_Right);
    DeltaSlot->SetVerticalAlignment(VAlign_Center);
    DeltaSlot->SetPadding(FMargin(0.0f, 0.0f, 2.0f, 0.0f));

    FSlateFontInfo DeltaFont = Entry.DeltaText->GetFont();
    DeltaFont.Size = 17;
    DeltaFont.TypefaceFontName = TEXT("Bold");
    Entry.DeltaText->SetFont(DeltaFont);
    Entry.DeltaText->SetShadowOffset(FVector2D(1.0f, 1.0f));
    Entry.DeltaText->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.95f));
    Entry.DeltaText->SetRenderOpacity(0.0f);
    Entry.DeltaText->SetVisibility(ESlateVisibility::Collapsed);
}

void UMortisGoldChangeToastWidget::BindSubsystemDelegates()
{
    if (RunStateSubsystemRef)
    {
        RunStateSubsystemRef->OnGoldChanged.RemoveDynamic(this, &ThisClass::HandleGoldChanged);
        RunStateSubsystemRef->OnGoldChanged.AddDynamic(this, &ThisClass::HandleGoldChanged);
    }

    if (MetaProgressionSubsystemRef)
    {
        MetaProgressionSubsystemRef->OnMemoryFragmentsChanged.RemoveDynamic(this, &ThisClass::HandleMemoryFragmentsChanged);
        MetaProgressionSubsystemRef->OnMemoryFragmentsChanged.AddDynamic(this, &ThisClass::HandleMemoryFragmentsChanged);
    }
}

void UMortisGoldChangeToastWidget::UnbindSubsystemDelegates()
{
    if (RunStateSubsystemRef)
    {
        RunStateSubsystemRef->OnGoldChanged.RemoveDynamic(this, &ThisClass::HandleGoldChanged);
    }

    if (MetaProgressionSubsystemRef)
    {
        MetaProgressionSubsystemRef->OnMemoryFragmentsChanged.RemoveDynamic(this, &ThisClass::HandleMemoryFragmentsChanged);
    }
}

void UMortisGoldChangeToastWidget::QueueCurrencyChange(EMortisCurrencyToastKind Kind, int32 Delta, int32 TargetTotal)
{
    if (Delta == 0)
    {
        return;
    }

    if (Kind == EMortisCurrencyToastKind::MemoryFragments)
    {
        PendingMemoryDelta += Delta;
        PendingMemoryTarget = TargetTotal;
        bHasPendingMemoryFragments = true;
    }
    else
    {
        PendingGoldDelta += Delta;
        PendingGoldTarget = TargetTotal;
        bHasPendingGold = true;
    }

    CoalesceElapsed = 0.0f;
    bIsCoalescing = true;
    ShowPanel();

    if (!IsDesignTime())
    {
        UE_LOG(LogMortisCurrencyToast, Log, TEXT("Queued currency toast. Kind=%s Delta=%d Target=%d"),
            Kind == EMortisCurrencyToastKind::MemoryFragments ? TEXT("MemoryFragments") : TEXT("Gold"),
            Delta,
            TargetTotal);
    }
}

void UMortisGoldChangeToastWidget::FlushPendingCurrencyChanges()
{
    bIsCoalescing = false;
    CoalesceElapsed = 0.0f;

    if (bHasPendingMemoryFragments)
    {
        StartEntryAnimation(MemoryFragmentsEntry, PendingMemoryDelta, PendingMemoryTarget);
        PendingMemoryDelta = 0;
        bHasPendingMemoryFragments = false;
    }

    if (bHasPendingGold)
    {
        StartEntryAnimation(GoldEntry, PendingGoldDelta, PendingGoldTarget);
        PendingGoldDelta = 0;
        bHasPendingGold = false;
    }

    UpdateEntryLayout();
    RefreshVisual();
}

void UMortisGoldChangeToastWidget::StartEntryAnimation(FMortisCurrencyToastEntry& Entry, int32 Delta, int32 TargetTotal)
{
    if (Entry.bIsAnimating)
    {
        UpdateEntryAnimation(Entry, 0.0f);
    }

    Entry.StartTotal = Entry.DisplayedTotal;
    Entry.TargetTotal = TargetTotal;
    Entry.PendingDelta += Delta;
    Entry.AnimationElapsed = 0.0f;
    Entry.bIsAnimating = true;
    Entry.bIsActive = true;

    if (Entry.RootBox)
    {
        Entry.RootBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }

    if (Entry.DeltaText)
    {
        Entry.DeltaText->SetRenderTranslation(DeltaVisibleTranslation);
        Entry.DeltaText->SetRenderOpacity(1.0f);
        Entry.DeltaText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }

    ShowPanel();
}

void UMortisGoldChangeToastWidget::UpdateEntryAnimation(FMortisCurrencyToastEntry& Entry, float InDeltaTime)
{
    Entry.AnimationElapsed += InDeltaTime;

    const float AbsorbStartTime = DeltaAppearDuration;
    const float AbsorbEndTime = DeltaAppearDuration + AbsorbDuration;
    const float FinishTime = AbsorbEndTime + HoldDuration;

    if (Entry.AnimationElapsed < AbsorbStartTime)
    {
        Entry.DisplayedTotal = Entry.StartTotal;

        if (Entry.DeltaText)
        {
            Entry.DeltaText->SetRenderTranslation(DeltaVisibleTranslation);
            Entry.DeltaText->SetRenderOpacity(1.0f);
        }
    }
    else if (Entry.AnimationElapsed < AbsorbEndTime)
    {
        const float AbsorbAlpha = FMath::Clamp((Entry.AnimationElapsed - AbsorbStartTime) / FMath::Max(AbsorbDuration, UE_KINDA_SMALL_NUMBER), 0.0f, 1.0f);
        const float SmoothedAlpha = FMath::InterpEaseOut(0.0f, 1.0f, AbsorbAlpha, 2.0f);

        Entry.DisplayedTotal = FMath::RoundToInt(FMath::Lerp(static_cast<float>(Entry.StartTotal), static_cast<float>(Entry.TargetTotal), SmoothedAlpha));

        if (Entry.DeltaText)
        {
            Entry.DeltaText->SetRenderTranslation(FMath::Lerp(DeltaVisibleTranslation, DeltaAbsorbedTranslation, SmoothedAlpha));
            Entry.DeltaText->SetRenderOpacity(1.0f - SmoothedAlpha);
        }
    }
    else if (Entry.AnimationElapsed < FinishTime)
    {
        Entry.DisplayedTotal = Entry.TargetTotal;

        if (Entry.DeltaText)
        {
            Entry.DeltaText->SetRenderOpacity(0.0f);
            Entry.DeltaText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    else
    {
        FinishEntryAnimation(Entry);
        return;
    }

    RefreshVisual();
}

void UMortisGoldChangeToastWidget::FinishEntryAnimation(FMortisCurrencyToastEntry& Entry)
{
    Entry.DisplayedTotal = Entry.TargetTotal;
    Entry.StartTotal = Entry.TargetTotal;
    Entry.PendingDelta = 0;
    Entry.AnimationElapsed = 0.0f;
    Entry.bIsAnimating = false;
    Entry.bIsActive = false;

    if (Entry.DeltaText)
    {
        Entry.DeltaText->SetRenderOpacity(0.0f);
        Entry.DeltaText->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (Entry.RootBox)
    {
        Entry.RootBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    UpdateEntryLayout();
    RefreshVisual();
}

void UMortisGoldChangeToastWidget::RefreshEntryVisual(FMortisCurrencyToastEntry& Entry, UTexture2D* Icon) const
{
    if (Entry.IconImage && Icon)
    {
        Entry.IconImage->SetBrushFromTexture(Icon, false);
    }

    if (Entry.TotalText)
    {
        Entry.TotalText->SetText(FText::AsNumber(Entry.DisplayedTotal));
    }

    if (Entry.DeltaText)
    {
        Entry.DeltaText->SetText(FText::FromString(FString::Printf(TEXT("%+d"), Entry.PendingDelta)));
        Entry.DeltaText->SetColorAndOpacity(FSlateColor(Entry.PendingDelta >= 0 ? GainDeltaColor : SpendDeltaColor));
    }
}

void UMortisGoldChangeToastWidget::UpdateEntryLayout()
{
    const bool bMemoryVisible = IsEntryVisible(MemoryFragmentsEntry);
    const bool bGoldVisible = IsEntryVisible(GoldEntry);
    const bool bDualVisible = bMemoryVisible && bGoldVisible;

    if (MemoryFragmentsEntry.Slot)
    {
        MemoryFragmentsEntry.Slot->SetPadding(FMargin(0.0f, 0.0f, bDualVisible ? EntrySpacing : 0.0f, 0.0f));
    }
}

void UMortisGoldChangeToastWidget::ShowPanel()
{
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UMortisGoldChangeToastWidget::HidePanelIfIdle()
{
    if (HasActiveEntries() || bIsCoalescing || bHasPendingGold || bHasPendingMemoryFragments)
    {
        return;
    }

    SetVisibility(ESlateVisibility::Hidden);
}

bool UMortisGoldChangeToastWidget::HasActiveEntries() const
{
    return IsEntryVisible(MemoryFragmentsEntry) || IsEntryVisible(GoldEntry);
}

bool UMortisGoldChangeToastWidget::IsEntryVisible(const FMortisCurrencyToastEntry& Entry) const
{
    return Entry.bIsActive || Entry.bIsAnimating;
}

FMortisCurrencyToastEntry& UMortisGoldChangeToastWidget::GetEntry(EMortisCurrencyToastKind Kind)
{
    return Kind == EMortisCurrencyToastKind::MemoryFragments ? MemoryFragmentsEntry : GoldEntry;
}
