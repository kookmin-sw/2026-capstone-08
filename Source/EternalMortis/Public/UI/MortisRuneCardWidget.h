#pragma once

#include "CoreMinimal.h"
#include "Types/MortisRuneDataTypes.h"
#include "UI/MortisRunePresentationStyle.h"
#include "UI/MortisWidgetBase.h"
#include "MortisRuneCardWidget.generated.h"

class UBorder;
class UButton;
class UImage;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortisOnRuneCardClicked, FMortisRuneInstance, ClickedRune);

USTRUCT(BlueprintType)
struct FMortisRuneCardVisualData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    bool bHasValidRuneData = false;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    TObjectPtr<UTexture2D> GlyphImage = nullptr;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    FLinearColor GlyphTint = FLinearColor::White;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    EMortisRuneGrade RuneGrade = EMortisRuneGrade::Common;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    TObjectPtr<UTexture2D> RuneGradeImage = nullptr;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    FLinearColor CardBackgroundTint = FLinearColor::Transparent;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    FLinearColor FrameTint = FLinearColor::Transparent;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    FLinearColor CoreTint = FLinearColor::White;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    FLinearColor GlyphGlowTint = FLinearColor::White;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    float GlyphGlowOpacity = 0.0f;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    float GlyphScale = 0.70f;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    float GradeImageOpacity = 0.0f;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    FMortisRunePresentationStyle RunePresentationStyle;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    bool bSelected = false;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Rune")
    bool bEquipped = false;
};

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisRuneCardWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void ClearRuneData();

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void SetRuneInstance(const FMortisRuneInstance& InRuneInstance);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void SetDisplayIcon(UTexture2D* InDisplayIcon);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void SetDisplayIconTint(const FLinearColor& InDisplayIconTint);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void ApplyData(const FMortisRuneInstance& InRuneInstance, UTexture2D* InDisplayIcon, const FLinearColor& InDisplayIconTint, bool bInSelected, bool bInEquipped);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void SetSelected(bool bInSelected);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void SetEquipped(bool bInEquipped);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void RefreshVisualState();

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void RefreshVisual();

    UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Rune")
    FMortisRuneCardVisualData GetRuneCardVisualData() const;

    UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Rune")
    UTexture2D* GetRuneGlyphImage() const;

    UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Rune")
    FLinearColor GetRuneGlyphTint() const;

    UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Rune")
    EMortisRuneGrade GetRuneGrade() const;

    UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Rune")
    UTexture2D* GetRuneGradeImage() const;

    UPROPERTY(BlueprintAssignable, Category = "Mortis|Inventory|Rune")
    FMortisOnRuneCardClicked OnRuneCardClicked;

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void HandleRootButtonClicked();

    UFUNCTION(BlueprintImplementableEvent, Category = "Mortis|Inventory|Rune")
    void ReceiveRunePresentationStyleChanged(const FMortisRunePresentationStyle& InStyle);

    UFUNCTION(BlueprintImplementableEvent, Category = "Mortis|Inventory|Rune")
    void ReceiveRuneVisualDataChanged(const FMortisRuneCardVisualData& InVisualData);

    FLinearColor BuildCardBackgroundTint() const;
    float ResolveFrameAlpha() const;
    float ResolveGlowOpacityMultiplier() const;
    float ResolveGradeImageOpacity() const;
    UTexture2D* ResolveRuneGradeImage(EMortisRuneGrade InGrade) const;
    bool HasValidRuneData() const;

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UButton> Button_Root = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UBorder> Border_CardBg = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UBorder> Border_RuneFrame = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_RuneCore = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_RuneGlow = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_RuneIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FMortisRuneInstance RuneInstance;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> DisplayIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FLinearColor DisplayIconTint = FLinearColor::White;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FMortisRunePresentationStyle CurrentPresentationStyle;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FMortisRuneCardVisualData CurrentVisualData;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    bool bSelected = false;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    bool bEquipped = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> CommonGradeImage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> RareGradeImage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> EpicGradeImage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> LegendaryGradeImage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style|Tuning", meta = (AllowPrivateAccess = "true", ClampMin = "0.1", ClampMax = "1.0"))
    float GlyphScale = 0.70f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style|Tuning", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
    float IdleCardBgAlpha = 0.22f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style|Tuning", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
    float EquippedCardBgAlpha = 0.28f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style|Tuning", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
    float SelectedCardBgAlpha = 0.36f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style|Tuning", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
    float EquippedFrameAlpha = 0.45f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style|Tuning", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
    float SelectedFrameAlpha = 0.95f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style|Tuning", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
    float IdleGlowOpacityMultiplier = 0.35f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style|Tuning", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "2.0"))
    float EquippedGlowOpacityMultiplier = 0.55f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style|Tuning", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "2.0"))
    float SelectedGlowOpacityMultiplier = 0.80f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style|Tuning", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
    float EquippedGradeAlpha = 0.16f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Style|Tuning", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
    float SelectedGradeAlpha = 0.28f;
};
