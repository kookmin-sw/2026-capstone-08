// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MortisPickupPreviewWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Styling/CoreStyle.h"

namespace
{
	FLinearColor GetDefaultPreviewBackgroundColor()
	{
		return FLinearColor(0.03f, 0.03f, 0.04f, 0.82f);
	}

	FLinearColor GetDefaultSubtitleColor()
	{
		return FLinearColor(0.85f, 0.85f, 0.89f, 1.f);
	}

	FLinearColor GetDefaultDetailColor()
	{
		return FLinearColor(0.70f, 0.70f, 0.76f, 1.f);
	}
}

void UMortisPickupPreviewWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BuildWidgetTreeIfNeeded();

	if (Border_Background)
	{
		Border_Background->SetClipping(EWidgetClipping::ClipToBounds);
	}

	if (Text_Title)
	{
		//Text_Title->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 18));
		Text_Title->SetAutoWrapText(true);
		Text_Title->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
		Text_Title->SetClipping(EWidgetClipping::ClipToBounds);
	}

	ClearPreview();
}

void UMortisPickupPreviewWidget::ApplyPreviewData(const FMortisPickupPreviewData& InPreviewData)
{
	CurrentPreviewData = InPreviewData;
	bHasPreviewData = true;

	RefreshPreviewState();
	BP_OnPreviewDataChanged(CurrentPreviewData, bHasPreviewData);
}

void UMortisPickupPreviewWidget::ClearPreview()
{
	CurrentPreviewData = FMortisPickupPreviewData();
	bHasPreviewData = false;

	RefreshPreviewState();
	BP_OnPreviewDataChanged(CurrentPreviewData, bHasPreviewData);
}

void UMortisPickupPreviewWidget::BuildWidgetTreeIfNeeded()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	bUsingNativeFallbackLayout = true;

	Border_Background = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_Background"));
	Border_Background->SetPadding(FMargin(14.f, 10.f));
	Border_Background->SetBrushColor(GetDefaultPreviewBackgroundColor());

	RootVerticalBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PreviewRootVBox"));
	Text_Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Text_Title"));

	if (Text_Title)
	{
		Text_Title->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 18));
		Text_Title->SetAutoWrapText(true);
	}
	if (RootVerticalBox && Text_Title)
	{
		if (UVerticalBoxSlot* VerticalBoxSlot = RootVerticalBox->AddChildToVerticalBox(Text_Title))
		{
			VerticalBoxSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 4.f));
		}
	}

	if (Border_Background && RootVerticalBox)
	{
		if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Border_Background->AddChild(RootVerticalBox)))
		{
			BorderSlot->SetHorizontalAlignment(HAlign_Fill);
			BorderSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}

	WidgetTree->RootWidget = Border_Background;
}

void UMortisPickupPreviewWidget::RefreshPreviewState()
{
	BuildWidgetTreeIfNeeded();

	if (Border_Background)
	{
		const FLinearColor BackgroundColor = bHasPreviewData
			? CurrentPreviewData.AccentColor.CopyWithNewOpacity(0.18f)
			: GetDefaultPreviewBackgroundColor();
		Border_Background->SetBrushColor(BackgroundColor);
	}
	
	if (Text_Title)
	{
		const bool bHasTitle = bHasPreviewData && !CurrentPreviewData.TitleText.IsEmpty();
		Text_Title->SetText(bHasTitle ? CurrentPreviewData.TitleText : FText::GetEmpty());
		Text_Title->SetColorAndOpacity(FSlateColor(bHasPreviewData ? CurrentPreviewData.AccentColor : FLinearColor::White));
		Text_Title->SetVisibility(bHasTitle ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}
