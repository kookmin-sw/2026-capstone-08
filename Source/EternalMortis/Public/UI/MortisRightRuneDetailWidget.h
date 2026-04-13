#pragma once

#include "CoreMinimal.h"
#include "UI/MortisRunePresentationStyle.h"
#include "UI/MortisRuneSynergyEntryWidget.h"
#include "UI/MortisWidgetBase.h"
#include "MortisRightRuneDetailWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UVerticalBox;
class UWidgetSwitcher;
class UTexture2D;
class UMortisRuneSynergyEntryWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMortisOnEquipRequested);

USTRUCT(BlueprintType)
struct FMortisRightRuneDetailViewData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    TObjectPtr<UTexture2D> RuneIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FLinearColor RuneIconTint = FLinearColor::White;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FMortisRunePresentationStyle RunePresentationStyle;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FText RuneName;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FText RuneStatValue;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FText SetName;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FText SetProgress;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    TArray<FMortisRuneSynergyEntryViewData> SynergyEntries;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    bool bShowEquipAction = false;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    bool bCanEquip = false;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FText EquipLabel;
};

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisRightRuneDetailWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void ApplyData(const FMortisRightRuneDetailViewData& InData);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void RefreshVisual();

    UPROPERTY(BlueprintAssignable, Category = "Mortis|Inventory|Rune")
    FMortisOnEquipRequested OnEquipRequested;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void HandleEquipButtonClicked();

    UFUNCTION(BlueprintImplementableEvent, Category = "Mortis|Inventory|Rune")
    void ReceiveRunePresentationStyleChanged(const FMortisRunePresentationStyle& InStyle);

    void RefreshSynergyEntries();

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_SelectedRuneCore = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_SelectedRuneGlow = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_SelectedRuneIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_SelectedRuneName = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_RuneStatValue = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_SelectedSetName = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_SelectedSetProgress = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_RuneSynergyTitle = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UVerticalBox> VerticalBox_RuneSynergyListBody = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UWidgetSwitcher> WidgetSwitcher_ActionState = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UButton> Button_EquipRune = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_EquipRuneLabel = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UMortisRuneSynergyEntryWidget> RuneSynergyEntryClass;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FMortisRightRuneDetailViewData CurrentData;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FMortisRunePresentationStyle CurrentPresentationStyle;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMortisRuneSynergyEntryWidget>> CreatedSynergyEntries;
};
