#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "MortisOptionsMenuWidget.generated.h"

class UButton;
class UComboBoxString;
class UTextBlock;
class USlider;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMortisOptionsMenuSimpleDelegate);

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisOptionsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Options")
	void RefreshFromSavedSettings();

	UPROPERTY(BlueprintAssignable, Category = "Mortis|Options")
	FMortisOptionsMenuSimpleDelegate OnBackRequested;

	UPROPERTY(BlueprintAssignable, Category = "Mortis|Options")
	FMortisOptionsMenuSimpleDelegate OnForgetMemoriesApplied;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleBackClicked();

	UFUNCTION()
	void HandleApplyClicked();

	UFUNCTION()
	void HandleForgetMemoriesClicked();

	UFUNCTION()
	void HandleResetCancelClicked();

	UFUNCTION()
	void HandleResetConfirmClicked();

	UFUNCTION()
	void HandleMasterVolumeChanged(float NewValue);

	UFUNCTION()
	void HandleResolutionSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void HandleRefreshModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

private:
	void BuildFallbackWidgetTreeIfNeeded();
	void CacheWidgets();
	void BindWidgetEvents();
	void UnbindWidgetEvents();

	void PopulateOptionLists();
	void ApplyPendingVideoSettings() const;
	void SaveMasterVolume() const;
	void ApplyMasterVolume(float Volume) const;
	void UpdateMasterVolumeText();
	void ShowResetConfirmation(bool bShow);
	void ResetProgression();

	FString ResolutionToString(const FIntPoint& Resolution) const;
	bool TryParseResolution(const FString& Option, FIntPoint& OutResolution) const;
	FString RefreshLimitToOption(bool bVSyncEnabled, float FrameRateLimit) const;
	float RefreshOptionToFrameRateLimit(const FString& Option) const;

private:
	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<USlider> Slider_MasterVolume = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_MasterVolumeValue = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UComboBoxString> Combo_Resolution = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UComboBoxString> Combo_RefreshMode = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_ForgetMemories = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_Back = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_Apply = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_ResetCancel = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_ResetConfirm = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<class UBorder> BD_ResetModalLayer = nullptr;

	UPROPERTY(Transient)
	float PendingMasterVolume = 0.8f;

	UPROPERTY(Transient)
	FString PendingResolutionOption;

	UPROPERTY(Transient)
	FString PendingRefreshModeOption;
};
