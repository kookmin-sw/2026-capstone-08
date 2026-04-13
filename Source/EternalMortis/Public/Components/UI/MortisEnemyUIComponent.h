// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/MortisUIComponent.h"
#include "MortisEnemyUIComponent.generated.h"

class UProgressBar;
class UWidgetComponent;
class UMortisEnemyHealthBarWidget;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class ETERNALMORTIS_API UMortisEnemyUIComponent : public UMortisUIComponent
{
	GENERATED_BODY()

public:
	UMortisEnemyUIComponent();

	virtual void BeginPlay() override;

	void InitializeEnemyHealthBar(UWidgetComponent* InWidgetComponent);
	void SetCombatHUDVisible(bool bShouldShow);
	void RefreshHealthBar();

protected:
	UFUNCTION()
	void HandleHealthChanged(float NewHealth, float MaxHealth);

private:
	void CacheHealthBarReferences();
	void ApplyHealthBarVisuals(float NewHealth, float MaxHealth);
	void ApplyHealthBarVisibility() const;

	TWeakObjectPtr<UWidgetComponent> EnemyHealthBarWidgetComponent;
	TWeakObjectPtr<UMortisEnemyHealthBarWidget> EnemyHealthBarWidget;
	TWeakObjectPtr<UProgressBar> EnemyHealthProgressBar;

	bool bHasCombatTarget = false;
	bool bIsDead = false;
};
