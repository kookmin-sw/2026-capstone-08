#pragma once

#include "CoreMinimal.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"
#include "MortisMainMenuRootWidget.generated.h"

class UButton;
class UUserWidget;
class UWidget;
class UWidgetSwitcher;
class UMortisOptionsMenuWidget;
class UMortisUpgradeSelectWidget;

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisMainMenuRootWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleStartClicked();

	UFUNCTION()
	void HandleOptionsClicked();

	UFUNCTION()
	void HandleQuitClicked();

	UFUNCTION()
	void HandleOptionsBackRequested();

	UFUNCTION()
	void HandleForgetMemoriesApplied();

private:
	void CacheWidgets();
	void BindWidgetEvents();
	void UnbindWidgetEvents();

	template <typename WidgetType>
	WidgetType* FindWidgetByName(UUserWidget* OwnerWidget, FName WidgetName) const
	{
		return OwnerWidget && OwnerWidget->WidgetTree
			? Cast<WidgetType>(OwnerWidget->WidgetTree->FindWidget(WidgetName))
			: nullptr;
	}

	void ShowMainMenu();
	void ShowUpgradeSelect();
	void ShowOptions();
	void ShowScreen(FName ScreenName, int32 FallbackIndex);
	void QuitGame();

private:
	UPROPERTY(Transient)
	TObjectPtr<UWidgetSwitcher> RootSwitcher = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> MainMenuWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMortisUpgradeSelectWidget> UpgradeSelectWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMortisOptionsMenuWidget> OptionsMenuWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UButton> BTN_StartGame = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UButton> BTN_Options = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UButton> BTN_Quit = nullptr;
};
