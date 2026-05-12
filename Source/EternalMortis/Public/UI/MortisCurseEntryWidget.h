#pragma once

#include "CoreMinimal.h"
#include "Types/MortisCurseDataTypes.h"
#include "UI/MortisWidgetBase.h"
#include "MortisCurseEntryWidget.generated.h"

class UBorder;
class UButton;
class UImage;
class UTextBlock;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortisOnCurseEntryClicked, FMortisCurseInstance, ClickedCurse);

USTRUCT(BlueprintType)
struct FMortisCurseEntryViewData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Curse")
    FMortisCurseInstance Curse;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Curse")
    TObjectPtr<UTexture2D> Icon = nullptr;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Curse")
    FText DisplayName;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Curse")
    bool bSelected = false;
};

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisCurseEntryWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Curse")
    void ApplyData(const FMortisCurseEntryViewData& InData);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Curse")
    void SetSelected(bool bInSelected);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Curse")
    void RefreshVisual();

    UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Curse")
    FMortisCurseEntryViewData GetCurseEntryViewData() const { return CurrentData; }

    UPROPERTY(BlueprintAssignable, Category = "Mortis|Inventory|Curse")
    FMortisOnCurseEntryClicked OnCurseEntryClicked;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void HandleRootButtonClicked();

    UFUNCTION(BlueprintImplementableEvent, Category = "Mortis|Inventory|Curse")
    void ReceiveCurseEntryDataChanged(const FMortisCurseEntryViewData& InData);

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UButton> Button_Root = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UBorder> Border_ItemBg = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_CurseIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_CurseName = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Curse", meta = (AllowPrivateAccess = "true"))
    FMortisCurseEntryViewData CurrentData;
};
