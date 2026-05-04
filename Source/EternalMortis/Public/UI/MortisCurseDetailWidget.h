#pragma once

#include "CoreMinimal.h"
#include "Types/MortisCurseDataTypes.h"
#include "UI/MortisWidgetBase.h"
#include "MortisCurseDetailWidget.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;

USTRUCT(BlueprintType)
struct FMortisCurseDetailViewData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Curse")
    FMortisCurseInstance Curse;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Curse")
    TObjectPtr<UTexture2D> Icon = nullptr;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Curse")
    FText DisplayName;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|Inventory|Curse")
    FText Description;
};

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisCurseDetailWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Curse")
    void ApplyData(const FMortisCurseDetailViewData& InData);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Curse")
    void ClearData();

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Curse")
    void RefreshVisual();

    UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Curse")
    FMortisCurseDetailViewData GetCurseDetailViewData() const { return CurrentData; }

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Mortis|Inventory|Curse")
    void ReceiveCurseDetailDataChanged(const FMortisCurseDetailViewData& InData);

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_CurseIcon = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_CurseName = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_CurseDescription = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Curse", meta = (AllowPrivateAccess = "true"))
    FMortisCurseDetailViewData CurrentData;
};
