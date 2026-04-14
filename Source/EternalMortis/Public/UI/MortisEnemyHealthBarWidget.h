// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MortisWidgetBase.h"
#include "MortisEnemyHealthBarWidget.generated.h"

class UProgressBar;
class USizeBox;
class UMortisUIComponent;
class UOverlay;

USTRUCT(BlueprintType)
struct FMortisEnemyHealthBarData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "EnemyHealthBar")
	float CurrentHealth = 0.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "EnemyHealthBar")
	float MaxHealth = 0.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "EnemyHealthBar")
	float HealthPercent = 0.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "EnemyHealthBar")
	float DesiredBarWidth = 120.f;
};

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisEnemyHealthBarWidget : public UMortisWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Mortis|EnemyHealthBar")
	void InitializeFromUIComponent(UMortisUIComponent* InObservedUIComponent);

	UFUNCTION(BlueprintCallable, Category = "Mortis|EnemyHealthBar")
	void ApplyHealthBarData(float NewHealth, float MaxHealth);

	UFUNCTION(BlueprintCallable, Category = "Mortis|EnemyHealthBar")
	void ClearHealthBar();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Mortis|EnemyHealthBar", meta = (DisplayName = "On Enemy Health Bar Data Changed"))
	void BP_OnHealthBarDataChanged(const FMortisEnemyHealthBarData& InHealthBarData, bool bInHasValidHealthData);

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|EnemyHealthBar", meta = (AllowPrivateAccess = "true"))
	FMortisEnemyHealthBarData CurrentHealthBarData;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|EnemyHealthBar", meta = (AllowPrivateAccess = "true"))
	bool bHasValidHealthData = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|EnemyHealthBar", meta = (ClampMin = "1.0", AllowPrivateAccess = "true"))
	float ReferenceMaxHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|EnemyHealthBar", meta = (ClampMin = "1.0", AllowPrivateAccess = "true"))
	float ReferenceBarWidth = 120.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|EnemyHealthBar", meta = (ClampMin = "1.0", AllowPrivateAccess = "true"))
	float MinBarWidth = 80.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|EnemyHealthBar", meta = (ClampMin = "1.0", AllowPrivateAccess = "true"))
	float MaxBarWidth = 220.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|EnemyHealthBar", meta = (ClampMin = "1.0", AllowPrivateAccess = "true"))
	float DefaultBarHeight = 12.f;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<USizeBox> SizeBox_BarRoot = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UProgressBar> MyProgressBar = nullptr;

	UFUNCTION()
	void HandleObservedHealthChanged(float NewHealth, float MaxHealth);

private:
	void UnbindObservedUIComponent();
	void BuildWidgetTreeIfNeeded();
	void RefreshHealthBarState();
	float ComputeDesiredBarWidth(float MaxHealth) const;

	TWeakObjectPtr<UMortisUIComponent> ObservedUIComponent;

	UPROPERTY(Transient)
	TObjectPtr<UOverlay> RootOverlay = nullptr;

	UPROPERTY(Transient)
	bool bUsingNativeFallbackLayout = false;
};
