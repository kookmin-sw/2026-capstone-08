#include "UI/MortisRuneCardWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

namespace
{
constexpr TCHAR RuneStoneCompositeMaterialPath[] = TEXT("/Game/UI/Material/M_UI_RuneStoneComposite.M_UI_RuneStoneComposite");
constexpr TCHAR StoneTexParameterName[] = TEXT("StoneTex");
constexpr TCHAR GlyphTexParameterName[] = TEXT("GlyphTex");
constexpr TCHAR GlyphTintParameterName[] = TEXT("GlyphTint");
constexpr TCHAR GlyphScaleParameterName[] = TEXT("GlyphScale");
constexpr TCHAR GlyphGlowTintParameterName[] = TEXT("GlyphGlowTint");
constexpr TCHAR GlyphGlowOpacityParameterName[] = TEXT("GlyphGlowOpacity");
constexpr TCHAR TraceWidthParameterName[] = TEXT("TraceWidth");
constexpr TCHAR TraceDarknessParameterName[] = TEXT("TraceDarkness");
constexpr TCHAR TraceOpacityParameterName[] = TEXT("TraceOpacity");
}

void UMortisRuneCardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_Root)
    {
        Button_Root->OnClicked.RemoveDynamic(this, &ThisClass::HandleRootButtonClicked);
        Button_Root->OnClicked.AddDynamic(this, &ThisClass::HandleRootButtonClicked);
    }

    ApplyCardSizeOverride();
    RefreshVisualState();
}

void UMortisRuneCardWidget::ClearRuneData()
{
    RuneInstance = FMortisRuneInstance();
    DisplayIcon = nullptr;
    DisplayIconTint = FLinearColor::White;
    bSelected = false;
    bEquipped = false;
    SlotState = EMortisRuneCardSlotState::Rune;
}

void UMortisRuneCardWidget::SetRuneInstance(const FMortisRuneInstance& InRuneInstance)
{
    RuneInstance = InRuneInstance;
}

void UMortisRuneCardWidget::SetDisplayIcon(UTexture2D* InDisplayIcon)
{
    DisplayIcon = InDisplayIcon;
}

void UMortisRuneCardWidget::SetDisplayIconTint(const FLinearColor& InDisplayIconTint)
{
    DisplayIconTint = InDisplayIconTint;
}

void UMortisRuneCardWidget::ApplyData(const FMortisRuneInstance& InRuneInstance, UTexture2D* InDisplayIcon, const FLinearColor& InDisplayIconTint, bool bInSelected, bool bInEquipped)
{
    ClearRuneData();
    SetRuneInstance(InRuneInstance);
    SetDisplayIcon(InDisplayIcon);
    SetDisplayIconTint(InDisplayIconTint);
    SetSelected(bInSelected);
    SetEquipped(bInEquipped);
    RefreshVisualState();
}

void UMortisRuneCardWidget::SetSelected(bool bInSelected)
{
    bSelected = bInSelected;
}

void UMortisRuneCardWidget::SetEquipped(bool bInEquipped)
{
    bEquipped = bInEquipped;
}

void UMortisRuneCardWidget::SetSlotState(EMortisRuneCardSlotState InSlotState)
{
    SlotState = InSlotState;
}

void UMortisRuneCardWidget::SetCardSize(const FVector2D& InCardSize)
{
    CardSizeOverride.X = FMath::Max(0.0f, InCardSize.X);
    CardSizeOverride.Y = FMath::Max(0.0f, InCardSize.Y);
    bHasCardSizeOverride = CardSizeOverride.X > 0.0f && CardSizeOverride.Y > 0.0f;

    ApplyCardSizeOverride();
}

void UMortisRuneCardWidget::RefreshVisualState()
{
    CurrentVisualData = FMortisRuneCardVisualData();
    CurrentVisualData.GlyphScale = GlyphScale;
    CurrentVisualData.bSelected = bSelected;
    CurrentVisualData.bEquipped = bEquipped;
    CurrentVisualData.SlotState = SlotState;

    if (!HasValidRuneData())
    {
        CurrentPresentationStyle = FMortisRunePresentationStyle();
        CurrentPresentationStyle.CoreTint = FLinearColor::Transparent;
        CurrentPresentationStyle.CoreShadowTint = FLinearColor::Transparent;
        CurrentPresentationStyle.FrameTint = FLinearColor::Transparent;
        CurrentPresentationStyle.GlyphGlowTint = FLinearColor::Transparent;
        CurrentPresentationStyle.GlyphGlowOpacity = 0.0f;

        switch (SlotState)
        {
        case EMortisRuneCardSlotState::EmptyUnlocked:
            CurrentVisualData.CardBackgroundTint = FLinearColor(0.08f, 0.08f, 0.10f, 0.68f);
            CurrentVisualData.FrameTint = FLinearColor(0.34f, 0.34f, 0.40f, EmptySlotFrameAlpha);
            break;

        case EMortisRuneCardSlotState::Locked:
            CurrentVisualData.CardBackgroundTint = FLinearColor(0.035f, 0.035f, 0.045f, 0.82f);
            CurrentVisualData.FrameTint = FLinearColor(0.16f, 0.16f, 0.18f, LockedSlotFrameAlpha);
            break;

        case EMortisRuneCardSlotState::Rune:
        default:
            CurrentVisualData.CardBackgroundTint = FLinearColor::Transparent;
            CurrentVisualData.FrameTint = FLinearColor::Transparent;
            break;
        }

        CurrentVisualData.CoreTint = FLinearColor::Transparent;
        CurrentVisualData.GlyphGlowTint = FLinearColor::Transparent;
        CurrentVisualData.RunePresentationStyle = CurrentPresentationStyle;

        if (Border_CardBg)
        {
            Border_CardBg->SetBrushColor(CurrentVisualData.CardBackgroundTint);
        }

        if (Border_RuneFrame)
        {
            Border_RuneFrame->SetBrushColor(CurrentVisualData.FrameTint);
        }

        if (Image_RuneCore)
        {
            Image_RuneCore->SetRenderOpacity(0.0f);
        }

        SetRenderOpacity(SlotState == EMortisRuneCardSlotState::Locked ? 0.55f : 1.0f);

        UpdateRuneCoreMaterial();
        ReceiveRunePresentationStyleChanged(CurrentPresentationStyle);
        ReceiveRuneVisualDataChanged(CurrentVisualData);
        return;
    }

    CurrentPresentationStyle = MortisRunePresentation::BuildStyle(RuneInstance.Grade, DisplayIconTint);

    CurrentVisualData.bHasValidRuneData = true;
    CurrentVisualData.GlyphImage = DisplayIcon;
    CurrentVisualData.GlyphTint = DisplayIcon ? CurrentPresentationStyle.GlyphTint : FLinearColor::White;
    CurrentVisualData.RuneGrade = RuneInstance.Grade;
    CurrentVisualData.RuneGradeImage = ResolveRuneGradeImage(RuneInstance.Grade);
    CurrentVisualData.CardBackgroundTint = BuildCardBackgroundTint();
    CurrentVisualData.FrameTint = CurrentPresentationStyle.FrameTint;
    CurrentVisualData.FrameTint.A = ResolveFrameAlpha();
    CurrentVisualData.CoreTint = CurrentPresentationStyle.CoreTint;
    CurrentVisualData.GlyphGlowTint = CurrentPresentationStyle.GlyphGlowTint;
    CurrentVisualData.GlyphGlowOpacity = DisplayIcon ? (CurrentPresentationStyle.GlyphGlowOpacity * ResolveGlowOpacityMultiplier()) : 0.0f;
    CurrentVisualData.GradeImageOpacity = CurrentVisualData.RuneGradeImage ? ResolveGradeImageOpacity() : 0.0f;
    CurrentVisualData.RunePresentationStyle = CurrentPresentationStyle;

    if (Border_CardBg)
    {
        Border_CardBg->SetBrushColor(CurrentVisualData.CardBackgroundTint);
    }

    if (Border_RuneFrame)
    {
        Border_RuneFrame->SetBrushColor(CurrentVisualData.FrameTint);
    }

    if (Image_RuneCore)
    {
        Image_RuneCore->SetRenderOpacity(1.0f);
    }

    SetRenderOpacity(1.0f);

    UpdateRuneCoreMaterial();
    ReceiveRunePresentationStyleChanged(CurrentPresentationStyle);
    ReceiveRuneVisualDataChanged(CurrentVisualData);
}

void UMortisRuneCardWidget::RefreshVisual()
{
    RefreshVisualState();
}

FMortisRuneCardVisualData UMortisRuneCardWidget::GetRuneCardVisualData() const
{
    return CurrentVisualData;
}

UTexture2D* UMortisRuneCardWidget::GetRuneGlyphImage() const
{
    return CurrentVisualData.GlyphImage;
}

FLinearColor UMortisRuneCardWidget::GetRuneGlyphTint() const
{
    return CurrentVisualData.GlyphTint;
}

EMortisRuneGrade UMortisRuneCardWidget::GetRuneGrade() const
{
    return CurrentVisualData.RuneGrade;
}

UTexture2D* UMortisRuneCardWidget::GetRuneGradeImage() const
{
    return CurrentVisualData.RuneGradeImage;
}

FLinearColor UMortisRuneCardWidget::BuildCardBackgroundTint() const
{
    FLinearColor BaseColor = CurrentPresentationStyle.CoreShadowTint;

    if (bSelected)
    {
        BaseColor = FLinearColor::LerpUsingHSV(CurrentPresentationStyle.CoreShadowTint, CurrentPresentationStyle.CoreTint, 0.20f);
        BaseColor.A = SelectedCardBgAlpha;
        return BaseColor;
    }

    if (bEquipped)
    {
        BaseColor = FLinearColor::LerpUsingHSV(CurrentPresentationStyle.CoreShadowTint, CurrentPresentationStyle.CoreTint, 0.10f);
        BaseColor.A = EquippedCardBgAlpha;
        return BaseColor;
    }

    BaseColor.A = IdleCardBgAlpha;
    return BaseColor;
}

float UMortisRuneCardWidget::ResolveFrameAlpha() const
{
    if (bSelected)
    {
        return SelectedFrameAlpha;
    }

    if (bEquipped)
    {
        return EquippedFrameAlpha;
    }

    return 0.0f;
}

float UMortisRuneCardWidget::ResolveGlowOpacityMultiplier() const
{
    if (bSelected)
    {
        return SelectedGlowOpacityMultiplier;
    }

    if (bEquipped)
    {
        return EquippedGlowOpacityMultiplier;
    }

    return IdleGlowOpacityMultiplier;
}

float UMortisRuneCardWidget::ResolveGradeImageOpacity() const
{
    if (bSelected)
    {
        return SelectedGradeAlpha;
    }

    if (bEquipped)
    {
        return EquippedGradeAlpha;
    }

    return 0.0f;
}

void UMortisRuneCardWidget::HandleRootButtonClicked()
{
    if (HasValidRuneData())
    {
        OnRuneCardClicked.Broadcast(RuneInstance);
    }
}

UTexture2D* UMortisRuneCardWidget::ResolveRuneGradeImage(EMortisRuneGrade InGrade) const
{
    switch (InGrade)
    {
    case EMortisRuneGrade::Rare:
        return RareGradeImage;

    case EMortisRuneGrade::Epic:
        return EpicGradeImage;

    case EMortisRuneGrade::Legendary:
        return LegendaryGradeImage;

    case EMortisRuneGrade::Common:
    default:
        return CommonGradeImage;
    }
}

bool UMortisRuneCardWidget::HasValidRuneData() const
{
    return RuneInstance.InstanceId.IsValid();
}

void UMortisRuneCardWidget::ApplyCardSizeOverride()
{
    if (!bHasCardSizeOverride)
    {
        return;
    }

    USizeBox* TargetSizeBox = SizeBox_Root;
    if (!TargetSizeBox)
    {
        TargetSizeBox = Cast<USizeBox>(GetRootWidget());
    }

    if (!TargetSizeBox)
    {
        return;
    }

    TargetSizeBox->SetWidthOverride(CardSizeOverride.X);
    TargetSizeBox->SetHeightOverride(CardSizeOverride.Y);
}

UMaterialInstanceDynamic* UMortisRuneCardWidget::ResolveRuneCoreMaterial()
{
    if (RuneCoreMaterial)
    {
        return RuneCoreMaterial;
    }

    if (!Image_RuneCore)
    {
        return nullptr;
    }

    RuneCoreMaterial = Image_RuneCore->GetDynamicMaterial();
    if (RuneCoreMaterial)
    {
        return RuneCoreMaterial;
    }

    if (UMaterialInterface* FallbackMaterial = LoadObject<UMaterialInterface>(nullptr, RuneStoneCompositeMaterialPath))
    {
        Image_RuneCore->SetBrushFromMaterial(FallbackMaterial);
        RuneCoreMaterial = Image_RuneCore->GetDynamicMaterial();
    }

    return RuneCoreMaterial;
}

void UMortisRuneCardWidget::UpdateRuneCoreMaterial()
{
    UMaterialInstanceDynamic* MaterialInstance = ResolveRuneCoreMaterial();
    if (!MaterialInstance)
    {
        return;
    }

    MaterialInstance->SetTextureParameterValue(StoneTexParameterName, CurrentVisualData.RuneGradeImage);
    MaterialInstance->SetTextureParameterValue(GlyphTexParameterName, CurrentVisualData.GlyphImage);
    MaterialInstance->SetVectorParameterValue(GlyphTintParameterName, CurrentVisualData.GlyphTint);
    MaterialInstance->SetScalarParameterValue(GlyphScaleParameterName, CurrentVisualData.GlyphScale);
    MaterialInstance->SetVectorParameterValue(GlyphGlowTintParameterName, CurrentVisualData.GlyphGlowTint);
    MaterialInstance->SetScalarParameterValue(GlyphGlowOpacityParameterName, CurrentVisualData.GlyphGlowOpacity);
    MaterialInstance->SetScalarParameterValue(TraceWidthParameterName, TraceWidth);
    MaterialInstance->SetScalarParameterValue(TraceDarknessParameterName, TraceDarkness);
    MaterialInstance->SetScalarParameterValue(TraceOpacityParameterName, TraceOpacity);
}
