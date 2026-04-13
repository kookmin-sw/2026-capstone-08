#pragma once

#include "CoreMinimal.h"
#include "Types/MortisRuneDataTypes.h"
#include "MortisRunePresentationStyle.generated.h"

USTRUCT(BlueprintType)
struct FMortisRunePresentationStyle
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    EMortisRuneGrade Grade = EMortisRuneGrade::Common;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    FLinearColor CoreTint = FLinearColor(0.33f, 0.34f, 0.36f, 1.0f);

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    FLinearColor CoreShadowTint = FLinearColor(0.10f, 0.10f, 0.12f, 1.0f);

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    FLinearColor FrameTint = FLinearColor(0.46f, 0.47f, 0.50f, 0.90f);

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    FLinearColor GlyphTint = FLinearColor::White;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    FLinearColor GlyphGlowTint = FLinearColor::White;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    float GlyphGlowOpacity = 0.35f;
};

namespace MortisRunePresentation
{
    FORCEINLINE FLinearColor BuildGlyphGlowTint(const FLinearColor& GlyphTint)
    {
        const FLinearColor SafeGlyphTint = GlyphTint.A > 0.0f ? GlyphTint : FLinearColor::White;
        FLinearColor GlowTint = FLinearColor::LerpUsingHSV(SafeGlyphTint, FLinearColor::White, 0.35f);
        GlowTint.A = 1.0f;
        return GlowTint;
    }

    FORCEINLINE FMortisRunePresentationStyle BuildStyle(EMortisRuneGrade Grade, const FLinearColor& GlyphTint)
    {
        FMortisRunePresentationStyle Style;
        Style.Grade = Grade;
        Style.GlyphTint = GlyphTint;
        Style.GlyphGlowTint = BuildGlyphGlowTint(GlyphTint);

        switch (Grade)
        {
        case EMortisRuneGrade::Rare:
            Style.CoreTint = FLinearColor(0.20f, 0.27f, 0.39f, 1.0f);
            Style.CoreShadowTint = FLinearColor(0.08f, 0.11f, 0.17f, 1.0f);
            Style.FrameTint = FLinearColor(0.41f, 0.59f, 0.82f, 0.92f);
            Style.GlyphGlowOpacity = 0.45f;
            break;

        case EMortisRuneGrade::Epic:
            Style.CoreTint = FLinearColor(0.31f, 0.23f, 0.38f, 1.0f);
            Style.CoreShadowTint = FLinearColor(0.10f, 0.08f, 0.14f, 1.0f);
            Style.FrameTint = FLinearColor(0.72f, 0.54f, 0.86f, 0.94f);
            Style.GlyphGlowOpacity = 0.55f;
            break;

        case EMortisRuneGrade::Legendary:
            Style.CoreTint = FLinearColor(0.47f, 0.36f, 0.19f, 1.0f);
            Style.CoreShadowTint = FLinearColor(0.17f, 0.11f, 0.04f, 1.0f);
            Style.FrameTint = FLinearColor(0.94f, 0.77f, 0.38f, 0.96f);
            Style.GlyphGlowOpacity = 0.65f;
            break;

        case EMortisRuneGrade::Common:
        default:
            Style.CoreTint = FLinearColor(0.33f, 0.34f, 0.36f, 1.0f);
            Style.CoreShadowTint = FLinearColor(0.10f, 0.10f, 0.12f, 1.0f);
            Style.FrameTint = FLinearColor(0.46f, 0.47f, 0.50f, 0.90f);
            Style.GlyphGlowOpacity = 0.35f;
            break;
        }

        return Style;
    }
}
