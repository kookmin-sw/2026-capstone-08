#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/MortisWidgetBase.h"
#include "MortisRuneSetEntryWidget.generated.h"

class UBorder;
class UButton;
class UImage;
class UTextBlock;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortisOnRuneSetEntryClicked, FGameplayTag, ClickedSetTag);

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisRuneSetEntryWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void ApplyData(FGameplayTag InSetTag, UTexture2D* InDisplayIcon, const FText& InDisplayName, int32 InOwnedCount, bool bInSelected);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void SetSelected(bool bInSelected);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void RefreshVisual();

    UPROPERTY(BlueprintAssignable, Category = "Mortis|Inventory|Rune")
    FMortisOnRuneSetEntryClicked OnSetEntryClicked;

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void HandleRootButtonClicked();

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UButton> Button_Root = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UBorder> Border_ItemBg = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_RuneSetIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_RuneSetName = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_RuneOwned = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FGameplayTag SetTag;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> DisplayIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FText DisplayName;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    int32 OwnedCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    bool bSelected = false;
};
