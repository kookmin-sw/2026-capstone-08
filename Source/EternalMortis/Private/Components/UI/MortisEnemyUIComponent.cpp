// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UI/MortisEnemyUIComponent.h"

#include "AbilitySystem/Attributes/MortisAttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Character/MortisCharacterBase.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"
#include "UI/MortisEnemyHealthBarWidget.h"

namespace
{
	const FName EnemyHealthProgressBarName(TEXT("MyProgressBar"));
}

UMortisEnemyUIComponent::UMortisEnemyUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMortisEnemyUIComponent::BeginPlay()
{
	Super::BeginPlay();

	OnHealthChanged.AddDynamic(this, &ThisClass::HandleHealthChanged);
}

void UMortisEnemyUIComponent::InitializeEnemyHealthBar(UWidgetComponent* InWidgetComponent)
{
	EnemyHealthBarWidgetComponent = InWidgetComponent;

	CacheHealthBarReferences();
	RefreshHealthBar();
	SetCombatHUDVisible(false);
}

void UMortisEnemyUIComponent::SetCombatHUDVisible(bool bShouldShow)
{
	bHasCombatTarget = bShouldShow;

	if (bHasCombatTarget)
	{
		RefreshHealthBar();
	}

	ApplyHealthBarVisibility();
}

void UMortisEnemyUIComponent::RefreshHealthBar()
{
	const AMortisCharacterBase* OwnerCharacter = Cast<AMortisCharacterBase>(GetOwner());
	const UMortisAttributeSet* AttributeSet = OwnerCharacter ? OwnerCharacter->GetMortisAttributeSet() : nullptr;
	if (!AttributeSet)
	{
		return;
	}

	const float MaxHealth = AttributeSet->GetMaxHealth();
	const float CurrentHealth = FMath::Clamp(AttributeSet->GetCurrentHealth(), 0.f, MaxHealth);
	bIsDead = CurrentHealth <= 0.f || MaxHealth <= 0.f;

	ApplyHealthBarVisuals(CurrentHealth, MaxHealth);
	ApplyHealthBarVisibility();
}

void UMortisEnemyUIComponent::HandleHealthChanged(float NewHealth, float MaxHealth)
{
	bIsDead = NewHealth <= 0.f || MaxHealth <= 0.f;
	if (bIsDead)
	{
		bHasCombatTarget = false;
	}

	if (!EnemyHealthBarWidget.IsValid())
	{
		ApplyHealthBarVisuals(NewHealth, MaxHealth);
	}

	ApplyHealthBarVisibility();
}

void UMortisEnemyUIComponent::CacheHealthBarReferences()
{
	EnemyHealthBarWidget = nullptr;
	EnemyHealthProgressBar = nullptr;

	if (!EnemyHealthBarWidgetComponent.IsValid())
	{
		return;
	}

	EnemyHealthBarWidgetComponent->InitWidget();

	UUserWidget* UserWidget = EnemyHealthBarWidgetComponent->GetUserWidgetObject();
	if (!UserWidget || !UserWidget->WidgetTree)
	{
		return;
	}

	if (UMortisEnemyHealthBarWidget* HealthBarWidget = Cast<UMortisEnemyHealthBarWidget>(UserWidget))
	{
		EnemyHealthBarWidget = HealthBarWidget;
		HealthBarWidget->InitializeFromUIComponent(this);
	}

	if (UProgressBar* ProgressBar = Cast<UProgressBar>(UserWidget->WidgetTree->FindWidget(EnemyHealthProgressBarName)))
	{
		EnemyHealthProgressBar = ProgressBar;
	}
}

void UMortisEnemyUIComponent::ApplyHealthBarVisuals(float NewHealth, float MaxHealth)
{
	if (!EnemyHealthBarWidget.IsValid() && !EnemyHealthProgressBar.IsValid())
	{
		CacheHealthBarReferences();
	}

	if (UMortisEnemyHealthBarWidget* HealthBarWidget = EnemyHealthBarWidget.Get())
	{
		HealthBarWidget->ApplyHealthBarData(NewHealth, MaxHealth);
		return;
	}

	if (UProgressBar* ProgressBar = EnemyHealthProgressBar.Get())
	{
		const float HealthPercent = MaxHealth > 0.f ? FMath::Clamp(NewHealth / MaxHealth, 0.f, 1.f) : 0.f;
		ProgressBar->SetPercent(HealthPercent);
	}
}

void UMortisEnemyUIComponent::ApplyHealthBarVisibility() const
{
	if (UWidgetComponent* WidgetComponent = EnemyHealthBarWidgetComponent.Get())
	{
		WidgetComponent->SetVisibility(bHasCombatTarget && !bIsDead);
	}
}

