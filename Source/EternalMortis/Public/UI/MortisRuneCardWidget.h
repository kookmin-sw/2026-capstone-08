#pragma once

#include "CoreMinimal.h"
#include "Types/MortisRuneDataTypes.h"
#include "UI/MortisWidgetBase.h"
#include "MortisRuneCardWidget.generated.h"

class UBorder;
class UButton;
class UImage;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortisOnRuneCardClicked, FMortisRuneInstance, ClickedRune);

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisRuneCardWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void ApplyData(const FMortisRuneInstance& InRuneInstance, UTexture2D* InDisplayIcon, bool bInSelected, bool bInEquipped);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void SetSelected(bool bInSelected);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void SetEquipped(bool bInEquipped);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void RefreshVisual();

    UPROPERTY(BlueprintAssignable, Category = "Mortis|Inventory|Rune")
    FMortisOnRuneCardClicked OnRuneCardClicked;

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void HandleRootButtonClicked();

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UButton> Button_Root = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UBorder> Border_CardBg = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_RuneIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FMortisRuneInstance RuneInstance;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> DisplayIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    bool bSelected = false;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    bool bEquipped = false;
};
