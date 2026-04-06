#pragma once

#include "CoreMinimal.h"
#include "UI/MortisWidgetBase.h"
#include "MortisRuneSynergyEntryWidget.generated.h"

class UBorder;
class UImage;
class UTextBlock;
class UTexture2D;

USTRUCT(BlueprintType)
struct FMortisRuneSynergyEntryViewData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    TObjectPtr<UTexture2D> DisplayIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    int32 RequiredCount = 0;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FText TitleText;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FText DescriptionText;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    bool bIsActive = false;
};

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisRuneSynergyEntryWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void ApplyData(const FMortisRuneSynergyEntryViewData& InData);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void SetActiveState(bool bInActive);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void RefreshVisual();

protected:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UBorder> Border_EntryBg = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_SynergyIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_TierCount = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_SynergyTitle = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_SynergyDescription = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FMortisRuneSynergyEntryViewData CurrentData;
};
