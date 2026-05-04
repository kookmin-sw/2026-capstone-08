#include "UI/MortisInventoryCursePageWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Engine/GameInstance.h"
#include "System/MortisCurseDatabaseSubsystem.h"
#include "System/MortisCurseInventorySubsystem.h"

void UMortisInventoryCursePageWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeReferences();
    BindInventoryDelegates();
    BindBackgroundButtons();

    HideDetailPanel();
    RefreshAll();
}

void UMortisInventoryCursePageWidget::NativeDestruct()
{
    UnbindBackgroundButtons();
    UnbindInventoryDelegates();
    ClearCurseEntries();

    Super::NativeDestruct();
}

void UMortisInventoryCursePageWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    LayoutCurseEntries();
    UpdateFloatingEntries(InDeltaTime);
    PositionDetailNearSelectedEntry();
}

void UMortisInventoryCursePageWidget::RefreshAll()
{
    RefreshCurseEntries();
    UpdateEmptyState();
    UpdateDetailPanel();
}

void UMortisInventoryCursePageWidget::ClearSelectedCurse()
{
    SelectedCurse = FMortisCurseInstance();
    RefreshEntrySelectionStates();
    HideDetailPanel();
}

bool UMortisInventoryCursePageWidget::HasSelectedCurse() const
{
    return SelectedCurse.InstanceId.IsValid();
}

void UMortisInventoryCursePageWidget::HandleCurseEntryClicked(FMortisCurseInstance ClickedCurse)
{
    if (!ClickedCurse.InstanceId.IsValid())
    {
        ClearSelectedCurse();
        return;
    }

    if (IsSelectedCurse(ClickedCurse))
    {
        ClearSelectedCurse();
        return;
    }

    SelectedCurse = ClickedCurse;
    RefreshEntrySelectionStates();
    UpdateDetailPanel();
}

void UMortisInventoryCursePageWidget::HandleBackgroundCloseClicked()
{
    ClearSelectedCurse();
}

void UMortisInventoryCursePageWidget::HandleCurseAdded(const FMortisCurseInstance& AddedCurse)
{
    RefreshAll();
}

void UMortisInventoryCursePageWidget::HandleCurseRemoved(const FMortisCurseInstance& RemovedCurse)
{
    if (IsSelectedCurse(RemovedCurse))
    {
        SelectedCurse = FMortisCurseInstance();
    }

    RefreshAll();
}

void UMortisInventoryCursePageWidget::InitializeReferences()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        CurseInventorySubsystemRef = GameInstance->GetSubsystem<UMortisCurseInventorySubsystem>();
        CurseDatabaseSubsystemRef = GameInstance->GetSubsystem<UMortisCurseDatabaseSubsystem>();
    }
}

void UMortisInventoryCursePageWidget::BindInventoryDelegates()
{
    if (!CurseInventorySubsystemRef)
    {
        return;
    }

    CurseInventorySubsystemRef->OnCurseAdded.RemoveDynamic(this, &ThisClass::HandleCurseAdded);
    CurseInventorySubsystemRef->OnCurseRemoved.RemoveDynamic(this, &ThisClass::HandleCurseRemoved);

    CurseInventorySubsystemRef->OnCurseAdded.AddDynamic(this, &ThisClass::HandleCurseAdded);
    CurseInventorySubsystemRef->OnCurseRemoved.AddDynamic(this, &ThisClass::HandleCurseRemoved);
}

void UMortisInventoryCursePageWidget::UnbindInventoryDelegates()
{
    if (!CurseInventorySubsystemRef)
    {
        return;
    }

    CurseInventorySubsystemRef->OnCurseAdded.RemoveDynamic(this, &ThisClass::HandleCurseAdded);
    CurseInventorySubsystemRef->OnCurseRemoved.RemoveDynamic(this, &ThisClass::HandleCurseRemoved);
}

void UMortisInventoryCursePageWidget::BindBackgroundButtons()
{
    if (Button_BackgroundClose)
    {
        Button_BackgroundClose->OnClicked.RemoveDynamic(this, &ThisClass::HandleBackgroundCloseClicked);
        Button_BackgroundClose->OnClicked.AddDynamic(this, &ThisClass::HandleBackgroundCloseClicked);
    }

    if (Button_CloseDetailArea)
    {
        Button_CloseDetailArea->OnClicked.RemoveDynamic(this, &ThisClass::HandleBackgroundCloseClicked);
        Button_CloseDetailArea->OnClicked.AddDynamic(this, &ThisClass::HandleBackgroundCloseClicked);
    }
}

void UMortisInventoryCursePageWidget::UnbindBackgroundButtons()
{
    if (Button_BackgroundClose)
    {
        Button_BackgroundClose->OnClicked.RemoveDynamic(this, &ThisClass::HandleBackgroundCloseClicked);
    }

    if (Button_CloseDetailArea)
    {
        Button_CloseDetailArea->OnClicked.RemoveDynamic(this, &ThisClass::HandleBackgroundCloseClicked);
    }
}

void UMortisInventoryCursePageWidget::RefreshCurseEntries()
{
    ClearCurseEntries();

    if (!Canvas_CurseField || !CurseEntryClass || !CurseInventorySubsystemRef)
    {
        return;
    }

    const TArray<FMortisCurseInstance>& OwnedCurses = CurseInventorySubsystemRef->GetOwnedCurses();
    if (HasSelectedCurse() && !DoesCurseExist(SelectedCurse))
    {
        SelectedCurse = FMortisCurseInstance();
    }

    for (int32 Index = 0; Index < OwnedCurses.Num(); ++Index)
    {
        const FMortisCurseInstance& Curse = OwnedCurses[Index];
        if (!Curse.InstanceId.IsValid())
        {
            continue;
        }

        UMortisCurseEntryWidget* EntryWidget = CreateWidget<UMortisCurseEntryWidget>(this, CurseEntryClass);
        if (!EntryWidget)
        {
            continue;
        }

        EntryWidget->ApplyData(BuildEntryViewData(Curse));
        EntryWidget->OnCurseEntryClicked.RemoveDynamic(this, &ThisClass::HandleCurseEntryClicked);
        EntryWidget->OnCurseEntryClicked.AddDynamic(this, &ThisClass::HandleCurseEntryClicked);

        if (UCanvasPanelSlot* CanvasSlot = Canvas_CurseField->AddChildToCanvas(EntryWidget))
        {
            CanvasSlot->SetAutoSize(false);
            CanvasSlot->SetSize(EntryWidgetSize);
            CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
            CanvasSlot->SetZOrder(10);
        }

        FMortisCurseEntryRuntimeData RuntimeData;
        RuntimeData.EntryWidget = EntryWidget;
        RuntimeData.Curse = Curse;
        RuntimeData.FloatPhase = static_cast<float>(CreatedCurseEntries.Num()) * 0.73f;
        CreatedCurseEntries.Add(RuntimeData);
    }

    LayoutCurseEntries();
}

void UMortisInventoryCursePageWidget::ClearCurseEntries()
{
    for (FMortisCurseEntryRuntimeData& RuntimeData : CreatedCurseEntries)
    {
        if (RuntimeData.EntryWidget)
        {
            RuntimeData.EntryWidget->OnCurseEntryClicked.RemoveDynamic(this, &ThisClass::HandleCurseEntryClicked);
            RuntimeData.EntryWidget->RemoveFromParent();
        }
    }

    CreatedCurseEntries.Reset();
}

void UMortisInventoryCursePageWidget::RefreshEntrySelectionStates()
{
    for (FMortisCurseEntryRuntimeData& RuntimeData : CreatedCurseEntries)
    {
        if (RuntimeData.EntryWidget)
        {
            RuntimeData.EntryWidget->SetSelected(IsSelectedCurse(RuntimeData.Curse));
        }
    }
}

void UMortisInventoryCursePageWidget::UpdateEmptyState()
{
    if (Text_EmptyMessage)
    {
        FText Message = NSLOCTEXT("MortisInventory", "CurseEmptyMessage", "No curses");
        bool bShouldShow = true;

        if (!Canvas_CurseField)
        {
            Message = NSLOCTEXT("MortisInventory", "CurseMissingCanvasField", "Canvas_CurseField is not bound");
        }
        else if (!CurseInventorySubsystemRef)
        {
            Message = NSLOCTEXT("MortisInventory", "CurseMissingInventorySubsystem", "Curse inventory subsystem is missing");
        }
        else if (!CurseEntryClass)
        {
            Message = NSLOCTEXT("MortisInventory", "CurseMissingEntryClass", "CurseEntryClass is not set");
        }
        else if (!CurseInventorySubsystemRef->GetOwnedCurses().IsEmpty())
        {
            bShouldShow = false;
        }

        Text_EmptyMessage->SetVisibility(bShouldShow ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
        Text_EmptyMessage->SetText(Message);
    }
}

void UMortisInventoryCursePageWidget::UpdateDetailPanel()
{
    if (!HasSelectedCurse() || !DoesCurseExist(SelectedCurse))
    {
        ClearSelectedCurse();
        return;
    }

    if (!Widget_CurseDetail)
    {
        return;
    }

    Widget_CurseDetail->ApplyData(BuildDetailViewData(SelectedCurse));
    Widget_CurseDetail->SetVisibility(ESlateVisibility::Visible);
    PositionDetailNearSelectedEntry();
}

void UMortisInventoryCursePageWidget::HideDetailPanel()
{
    if (Widget_CurseDetail)
    {
        Widget_CurseDetail->ClearData();
        Widget_CurseDetail->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UMortisInventoryCursePageWidget::LayoutCurseEntries()
{
    const int32 EntryCount = CreatedCurseEntries.Num();
    if (EntryCount <= 0)
    {
        return;
    }

    const FVector2D AnchorPosition = ResolveHeadAnchorPosition();

    if (EntryCount < SecondArcStartCount)
    {
        const float EntryScale = EntryCount >= ShrinkStartCount ? EntryScaleWhenCrowded : 1.0f;
        PlaceArcEntries(0, EntryCount, AnchorPosition, PrimaryRadius, PrimaryArcDegrees, EntryScale, 20);
        return;
    }

    const int32 PrimaryCount = FMath::Min(PrimaryArcCapacity, EntryCount);
    const int32 SecondCount = EntryCount - PrimaryCount;

    PlaceArcEntries(0, PrimaryCount, AnchorPosition, PrimaryRadius, PrimaryArcDegrees, 1.0f, 20);
    PlaceArcEntries(PrimaryCount, SecondCount, AnchorPosition, SecondRadius, PrimaryArcDegrees, SecondArcEntryScale, 10);
}

void UMortisInventoryCursePageWidget::PlaceArcEntries(int32 StartIndex, int32 EntryCount, const FVector2D& AnchorPosition, float Radius, float ArcDegrees, float EntryScale, int32 ZOrder)
{
    if (EntryCount <= 0)
    {
        return;
    }

    const float SafeArcDegrees = FMath::Clamp(ArcDegrees, 1.0f, 180.0f);
    const float StartAngleDegrees = 90.0f - (SafeArcDegrees * 0.5f);
    const float StepDegrees = EntryCount > 1 ? SafeArcDegrees / static_cast<float>(EntryCount - 1) : 0.0f;

    for (int32 LocalIndex = 0; LocalIndex < EntryCount; ++LocalIndex)
    {
        const int32 RuntimeIndex = StartIndex + LocalIndex;
        if (!CreatedCurseEntries.IsValidIndex(RuntimeIndex))
        {
            continue;
        }

        FMortisCurseEntryRuntimeData& RuntimeData = CreatedCurseEntries[RuntimeIndex];
        if (!RuntimeData.EntryWidget)
        {
            continue;
        }

        const float AngleDegrees = EntryCount > 1 ? StartAngleDegrees + (StepDegrees * LocalIndex) : 90.0f;
        const float AngleRadians = FMath::DegreesToRadians(AngleDegrees);
        RuntimeData.BasePosition = AnchorPosition + FVector2D(FMath::Cos(AngleRadians) * Radius, -FMath::Sin(AngleRadians) * Radius);
        RuntimeData.Scale = EntryScale;

        RuntimeData.EntryWidget->SetRenderScale(FVector2D(EntryScale, EntryScale));

        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(RuntimeData.EntryWidget->Slot))
        {
            CanvasSlot->SetPosition(RuntimeData.BasePosition);
            CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
            CanvasSlot->SetAutoSize(false);
            CanvasSlot->SetSize(EntryWidgetSize);
            CanvasSlot->SetZOrder(IsSelectedCurse(RuntimeData.Curse) ? ZOrder + 20 : ZOrder);
        }
    }
}

void UMortisInventoryCursePageWidget::UpdateFloatingEntries(float InDeltaTime)
{
    FloatingElapsedTime += InDeltaTime;

    for (FMortisCurseEntryRuntimeData& RuntimeData : CreatedCurseEntries)
    {
        if (!RuntimeData.EntryWidget)
        {
            continue;
        }

        const float Wave = FloatingElapsedTime * FloatSpeed + RuntimeData.FloatPhase;
        const FVector2D FloatingOffset(
            FMath::Cos(Wave * 0.73f) * FloatAmplitude * 0.28f,
            FMath::Sin(Wave) * FloatAmplitude);

        RuntimeData.EntryWidget->SetRenderTranslation(FloatingOffset);
        RuntimeData.EntryWidget->SetRenderScale(FVector2D(RuntimeData.Scale, RuntimeData.Scale));
    }
}

void UMortisInventoryCursePageWidget::PositionDetailNearSelectedEntry()
{
    if (!Widget_CurseDetail || Widget_CurseDetail->GetVisibility() == ESlateVisibility::Collapsed || !HasSelectedCurse())
    {
        return;
    }

    FMortisCurseEntryRuntimeData* RuntimeData = FindRuntimeDataByCurseId(SelectedCurse.InstanceId);
    if (!RuntimeData)
    {
        return;
    }

    UCanvasPanelSlot* DetailSlot = Cast<UCanvasPanelSlot>(Widget_CurseDetail->Slot);
    if (!DetailSlot)
    {
        return;
    }

    const FVector2D CanvasSize = ResolveCanvasSize();
    FVector2D DetailSize = Widget_CurseDetail->GetDesiredSize();
    if (DetailSize.IsNearlyZero())
    {
        DetailSize = DefaultDetailSize;
    }

    FVector2D DesiredPosition = RuntimeData->BasePosition + DetailPopupOffset;
    if (CanvasSize.X > 0.0f && DesiredPosition.X + DetailSize.X > CanvasSize.X)
    {
        DesiredPosition.X = RuntimeData->BasePosition.X - DetailPopupOffset.X - DetailSize.X;
    }

    if (CanvasSize.Y > 0.0f && DesiredPosition.Y + DetailSize.Y > CanvasSize.Y)
    {
        DesiredPosition.Y = RuntimeData->BasePosition.Y - DetailPopupOffset.Y - DetailSize.Y;
    }

    if (CanvasSize.X > 0.0f)
    {
        DesiredPosition.X = FMath::Clamp(DesiredPosition.X, 0.0f, FMath::Max(0.0f, CanvasSize.X - DetailSize.X));
    }

    if (CanvasSize.Y > 0.0f)
    {
        DesiredPosition.Y = FMath::Clamp(DesiredPosition.Y, 0.0f, FMath::Max(0.0f, CanvasSize.Y - DetailSize.Y));
    }

    DetailSlot->SetPosition(DesiredPosition);
    DetailSlot->SetAlignment(FVector2D::ZeroVector);
    DetailSlot->SetAutoSize(true);
    DetailSlot->SetZOrder(100);
}

FMortisCurseEntryViewData UMortisInventoryCursePageWidget::BuildEntryViewData(const FMortisCurseInstance& Curse) const
{
    FMortisCurseEntryViewData ViewData;
    ViewData.Curse = Curse;
    ViewData.bSelected = IsSelectedCurse(Curse);

    const FMortisCurseRow* CurseRow = GetCurseRow(Curse);
    ViewData.Icon = ResolveCurseIcon();

    if (CurseRow)
    {
        ViewData.DisplayName = CurseRow->DisplayName;
    }
    else
    {
        ViewData.DisplayName = FText::FromString(Curse.CurseTag.ToString());
    }

    return ViewData;
}

FMortisCurseDetailViewData UMortisInventoryCursePageWidget::BuildDetailViewData(const FMortisCurseInstance& Curse) const
{
    FMortisCurseDetailViewData ViewData;
    ViewData.Curse = Curse;

    const FMortisCurseRow* CurseRow = GetCurseRow(Curse);
    ViewData.Icon = ResolveCurseIcon();

    if (CurseRow)
    {
        ViewData.DisplayName = CurseRow->DisplayName;
        ViewData.Description = BuildCurseDescriptionText(Curse, CurseRow);
    }
    else
    {
        ViewData.DisplayName = FText::FromString(Curse.CurseTag.ToString());
        ViewData.Description = BuildCurseDescriptionText(Curse, nullptr);
    }

    return ViewData;
}

FText UMortisInventoryCursePageWidget::BuildCurseDescriptionText(const FMortisCurseInstance& Curse, const FMortisCurseRow* CurseRow) const
{
    const FText ValueText = FText::AsNumber(Curse.RolledValue);

    if (CurseRow && !CurseRow->DescriptionFormat.IsEmpty())
    {
        return FText::Format(FTextFormat::FromString(CurseRow->DescriptionFormat.ToString()), ValueText);
    }

    return FText::Format(
        NSLOCTEXT("MortisInventory", "CurseDescriptionFallback", "Value {0}"),
        ValueText);
}

UTexture2D* UMortisInventoryCursePageWidget::ResolveCurseIcon() const
{
    return FixedCurseIcon;
}

bool UMortisInventoryCursePageWidget::IsSelectedCurse(const FMortisCurseInstance& Curse) const
{
    return HasSelectedCurse() && Curse.InstanceId.IsValid() && SelectedCurse.InstanceId == Curse.InstanceId;
}

bool UMortisInventoryCursePageWidget::DoesCurseExist(const FMortisCurseInstance& Curse) const
{
    if (!CurseInventorySubsystemRef || !Curse.InstanceId.IsValid())
    {
        return false;
    }

    for (const FMortisCurseInstance& OwnedCurse : CurseInventorySubsystemRef->GetOwnedCurses())
    {
        if (OwnedCurse.InstanceId == Curse.InstanceId)
        {
            return true;
        }
    }

    return false;
}

FMortisCurseEntryRuntimeData* UMortisInventoryCursePageWidget::FindRuntimeDataByCurseId(const FGuid& CurseId)
{
    if (!CurseId.IsValid())
    {
        return nullptr;
    }

    return CreatedCurseEntries.FindByPredicate(
        [&CurseId](const FMortisCurseEntryRuntimeData& RuntimeData)
        {
            return RuntimeData.Curse.InstanceId == CurseId;
        });
}

const FMortisCurseRow* UMortisInventoryCursePageWidget::GetCurseRow(const FMortisCurseInstance& Curse) const
{
    if (!CurseDatabaseSubsystemRef || !Curse.CurseTag.IsValid())
    {
        return nullptr;
    }

    return CurseDatabaseSubsystemRef->GetCurseRow(Curse.CurseTag);
}

FVector2D UMortisInventoryCursePageWidget::ResolveHeadAnchorPosition() const
{
    if (Widget_HeadAnchor)
    {
        if (const UCanvasPanelSlot* AnchorSlot = Cast<UCanvasPanelSlot>(Widget_HeadAnchor->Slot))
        {
            const FVector2D Position = AnchorSlot->GetPosition();
            const FVector2D Size = AnchorSlot->GetSize();
            const FVector2D Alignment = AnchorSlot->GetAlignment();
            return Position + Size * (FVector2D(0.5f, 0.5f) - Alignment);
        }

        if (Canvas_CurseField)
        {
            const FGeometry& CanvasGeometry = Canvas_CurseField->GetCachedGeometry();
            const FGeometry& AnchorGeometry = Widget_HeadAnchor->GetCachedGeometry();
            if (!CanvasGeometry.GetLocalSize().IsNearlyZero() && !AnchorGeometry.GetLocalSize().IsNearlyZero())
            {
                const FVector2D AnchorTopLeft = CanvasGeometry.AbsoluteToLocal(AnchorGeometry.GetAbsolutePosition());
                return AnchorTopLeft + AnchorGeometry.GetLocalSize() * 0.5f;
            }
        }
    }

    const FVector2D CanvasSize = ResolveCanvasSize();
    return FVector2D(CanvasSize.X * 0.5f, CanvasSize.Y * 0.72f);
}

FVector2D UMortisInventoryCursePageWidget::ResolveCanvasSize() const
{
    if (!Canvas_CurseField)
    {
        return FVector2D::ZeroVector;
    }

    const FVector2D CachedSize = Canvas_CurseField->GetCachedGeometry().GetLocalSize();
    if (!CachedSize.IsNearlyZero())
    {
        return CachedSize;
    }

    return Canvas_CurseField->GetDesiredSize();
}
