// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MortisEnemyHealthBarWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/UI/MortisUIComponent.h"
#include "Styling/SlateColor.h"

namespace
{
	FLinearColor GetDefaultBarBackgroundColor()
	{
		return FLinearColor(0.03f, 0.03f, 0.04f, 0.65f);
	}

	FLinearColor GetDefaultBarFillColor()
	{
		return FLinearColor(0.72f, 0.10f, 0.10f, 0.95f);
	}
}

void UMortisEnemyHealthBarWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BuildWidgetTreeIfNeeded();
	ClearHealthBar();
}

void UMortisEnemyHealthBarWidget::NativeDestruct()
{
	UnbindObservedUIComponent();

	Super::NativeDestruct();
}

void UMortisEnemyHealthBarWidget::InitializeFromUIComponent(UMortisUIComponent* InObservedUIComponent)
{
	if (ObservedUIComponent.Get() == InObservedUIComponent)
	{
		return;
	}

	UnbindObservedUIComponent();
	ObservedUIComponent = InObservedUIComponent;

	if (UMortisUIComponent* UIComponent = ObservedUIComponent.Get())
	{
		UIComponent->OnHealthChanged.AddUniqueDynamic(this, &ThisClass::HandleObservedHealthChanged);
	}
}

void UMortisEnemyHealthBarWidget::ApplyHealthBarData(float NewHealth, float MaxHealth)
{
	const float SafeMaxHealth = FMath::Max(MaxHealth, 0.f);
	const float SafeCurrentHealth = SafeMaxHealth > 0.f
		? FMath::Clamp(NewHealth, 0.f, SafeMaxHealth)
		: 0.f;

	CurrentHealthBarData.CurrentHealth = SafeCurrentHealth;
	CurrentHealthBarData.MaxHealth = SafeMaxHealth;
	CurrentHealthBarData.HealthPercent = SafeMaxHealth > 0.f
		? FMath::Clamp(SafeCurrentHealth / SafeMaxHealth, 0.f, 1.f)
		: 0.f;
	CurrentHealthBarData.DesiredBarWidth = ComputeDesiredBarWidth(SafeMaxHealth);
	bHasValidHealthData = SafeMaxHealth > 0.f;

	RefreshHealthBarState();
	BP_OnHealthBarDataChanged(CurrentHealthBarData, bHasValidHealthData);
}

void UMortisEnemyHealthBarWidget::ClearHealthBar()
{
	CurrentHealthBarData = FMortisEnemyHealthBarData();
	CurrentHealthBarData.DesiredBarWidth = ReferenceBarWidth;
	bHasValidHealthData = false;

	RefreshHealthBarState();
	BP_OnHealthBarDataChanged(CurrentHealthBarData, bHasValidHealthData);
}

void UMortisEnemyHealthBarWidget::HandleObservedHealthChanged(float NewHealth, float MaxHealth)
{
	ApplyHealthBarData(NewHealth, MaxHealth);
}

void UMortisEnemyHealthBarWidget::UnbindObservedUIComponent()
{
	if (UMortisUIComponent* UIComponent = ObservedUIComponent.Get())
	{
		UIComponent->OnHealthChanged.RemoveDynamic(this, &ThisClass::HandleObservedHealthChanged);
	}

	ObservedUIComponent = nullptr;
}

void UMortisEnemyHealthBarWidget::BuildWidgetTreeIfNeeded()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	bUsingNativeFallbackLayout = true;

	SizeBox_BarRoot = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("SizeBox_BarRoot"));
	SizeBox_BarRoot->SetWidthOverride(ReferenceBarWidth);
	SizeBox_BarRoot->SetHeightOverride(DefaultBarHeight);

	RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("Overlay_Root"));
	MyProgressBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("MyProgressBar"));

	if (MyProgressBar)
	{
		FProgressBarStyle ProgressBarStyle = MyProgressBar->GetWidgetStyle();
		ProgressBarStyle.BackgroundImage.TintColor = FSlateColor(GetDefaultBarBackgroundColor());
		ProgressBarStyle.FillImage.TintColor = FSlateColor(GetDefaultBarFillColor());
		MyProgressBar->SetWidgetStyle(ProgressBarStyle);
		MyProgressBar->SetPercent(0.f);
	}

	if (RootOverlay && MyProgressBar)
	{
		RootOverlay->AddChildToOverlay(MyProgressBar);
	}

	if (SizeBox_BarRoot && RootOverlay)
	{
		SizeBox_BarRoot->SetContent(RootOverlay);
	}

	WidgetTree->RootWidget = SizeBox_BarRoot;
}

void UMortisEnemyHealthBarWidget::RefreshHealthBarState()
{
	BuildWidgetTreeIfNeeded();

	if (SizeBox_BarRoot)
	{
		SizeBox_BarRoot->SetWidthOverride(bHasValidHealthData ? CurrentHealthBarData.DesiredBarWidth : ReferenceBarWidth);

		if (bUsingNativeFallbackLayout)
		{
			SizeBox_BarRoot->SetHeightOverride(DefaultBarHeight);
		}
	}

	if (MyProgressBar)
	{
		MyProgressBar->SetPercent(bHasValidHealthData ? CurrentHealthBarData.HealthPercent : 0.f);
	}
}

float UMortisEnemyHealthBarWidget::ComputeDesiredBarWidth(float MaxHealth) const
{
	if (MaxHealth <= 0.f)
	{
		return ReferenceBarWidth;
	}

	const float SafeReferenceMaxHealth = FMath::Max(ReferenceMaxHealth, 1.f);
	const float WidthScale = MaxHealth / SafeReferenceMaxHealth;
	return FMath::Clamp(ReferenceBarWidth * WidthScale, MinBarWidth, MaxBarWidth);
}
