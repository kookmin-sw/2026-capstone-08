#include "UI/MortisRuneCardWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Styling/SlateColor.h"

void UMortisRuneCardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_Root)
    {
        Button_Root->OnClicked.RemoveDynamic(this, &ThisClass::HandleRootButtonClicked);
        Button_Root->OnClicked.AddDynamic(this, &ThisClass::HandleRootButtonClicked);
    }

    RefreshVisual();
}

void UMortisRuneCardWidget::ApplyData(const FMortisRuneInstance& InRuneInstance, UTexture2D* InDisplayIcon, const FLinearColor& InDisplayIconTint, bool bInSelected, bool bInEquipped)
{
    RuneInstance = InRuneInstance;
    DisplayIcon = InDisplayIcon;
    DisplayIconTint = InDisplayIconTint;
    bSelected = bInSelected;
    bEquipped = bInEquipped;

    RefreshVisual();
}

void UMortisRuneCardWidget::SetSelected(bool bInSelected)
{
    bSelected = bInSelected;
    RefreshVisual();
}

void UMortisRuneCardWidget::SetEquipped(bool bInEquipped)
{
    bEquipped = bInEquipped;
    RefreshVisual();
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

void UMortisRuneCardWidget::RefreshVisual()
{
    CurrentPresentationStyle = MortisRunePresentation::BuildStyle(RuneInstance.Grade, DisplayIconTint);

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
    CurrentVisualData.GlyphScale = GlyphScale;
    CurrentVisualData.GradeImageOpacity = CurrentVisualData.RuneGradeImage ? ResolveGradeImageOpacity() : 0.0f;
    CurrentVisualData.RunePresentationStyle = CurrentPresentationStyle;
    CurrentVisualData.bSelected = bSelected;
    CurrentVisualData.bEquipped = bEquipped;

    if (Image_RuneCore)
    {
        Image_RuneCore->SetBrushTintColor(FSlateColor(CurrentVisualData.CoreTint));
        Image_RuneCore->SetColorAndOpacity(CurrentVisualData.CoreTint);
    }

    if (Image_RuneGlow)
    {
        Image_RuneGlow->SetBrushFromTexture(DisplayIcon, false);
        Image_RuneGlow->SetBrushTintColor(FSlateColor(CurrentVisualData.GlyphGlowTint));
        Image_RuneGlow->SetColorAndOpacity(FLinearColor::White);
        Image_RuneGlow->SetRenderOpacity(CurrentVisualData.GlyphGlowOpacity);
        Image_RuneGlow->SetRenderScale(FVector2D(CurrentVisualData.GlyphScale, CurrentVisualData.GlyphScale));
    }

    if (Image_RuneIcon)
    {
        Image_RuneIcon->SetBrushFromTexture(DisplayIcon, false);

        // Keep the RuneSet tint on the brush itself so designer-side image color defaults
        // do not wash the glyph back to white at runtime.
        Image_RuneIcon->SetBrushTintColor(FSlateColor(CurrentVisualData.GlyphTint));
        Image_RuneIcon->SetColorAndOpacity(FLinearColor::White);
        Image_RuneIcon->SetRenderScale(FVector2D(CurrentVisualData.GlyphScale, CurrentVisualData.GlyphScale));
    }

    if (Border_CardBg)
    {
        Border_CardBg->SetBrushColor(CurrentVisualData.CardBackgroundTint);
    }

    if (Border_RuneFrame)
    {
        Border_RuneFrame->SetBrushColor(CurrentVisualData.FrameTint);
    }

    ReceiveRunePresentationStyleChanged(CurrentPresentationStyle);
    ReceiveRuneVisualDataChanged(CurrentVisualData);
}

void UMortisRuneCardWidget::HandleRootButtonClicked()
{
    OnRuneCardClicked.Broadcast(RuneInstance);
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
