// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MortisWidgetBase.h"
#include "MortisPickupPreviewWidget.generated.h"

class UBorder;
class UTextBlock;
class UVerticalBox;

USTRUCT(BlueprintType)
struct FMortisPickupPreviewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Preview")
	FText CategoryText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Preview")
	FText TitleText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Preview")
	FText SubtitleText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Preview")
	FText DetailText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Preview")
	FLinearColor AccentColor = FLinearColor(0.88f, 0.75f, 0.55f, 1.f);
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisPickupPreviewWidget : public UMortisWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintCallable, Category = "Mortis|PickupPreview")
	void ApplyPreviewData(const FMortisPickupPreviewData& InPreviewData);

	UFUNCTION(BlueprintCallable, Category = "Mortis|PickupPreview")
	void ClearPreview();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Mortis|PickupPreview", meta = (DisplayName = "On Preview Data Changed"))
	void BP_OnPreviewDataChanged(const FMortisPickupPreviewData& InPreviewData, bool bInHasPreviewData);

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|PickupPreview", meta = (AllowPrivateAccess = "true"))
	FMortisPickupPreviewData CurrentPreviewData;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|PickupPreview", meta = (AllowPrivateAccess = "true"))
	bool bHasPreviewData = false;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UBorder> Border_Background = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> Text_Category = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> Text_Title = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> Text_Subtitle = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> Text_Detail = nullptr;

private:
	void BuildWidgetTreeIfNeeded();
	void RefreshPreviewState();

	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> RootVerticalBox = nullptr;

	UPROPERTY(Transient)
	bool bUsingNativeFallbackLayout = false;
};
