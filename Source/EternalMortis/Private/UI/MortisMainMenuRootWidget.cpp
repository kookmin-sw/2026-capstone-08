#include "UI/MortisMainMenuRootWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/MortisOptionsMenuWidget.h"
#include "UI/MortisUpgradeSelectWidget.h"

void UMortisMainMenuRootWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CacheWidgets();
	BindWidgetEvents();
	ShowMainMenu();
}

void UMortisMainMenuRootWidget::NativeDestruct()
{
	UnbindWidgetEvents();

	Super::NativeDestruct();
}

void UMortisMainMenuRootWidget::HandleStartClicked()
{
	ShowUpgradeSelect();
}

void UMortisMainMenuRootWidget::HandleOptionsClicked()
{
	ShowOptions();
}

void UMortisMainMenuRootWidget::HandleQuitClicked()
{
	QuitGame();
}

void UMortisMainMenuRootWidget::HandleOptionsBackRequested()
{
	ShowMainMenu();
}

void UMortisMainMenuRootWidget::HandleForgetMemoriesApplied()
{
	if (UpgradeSelectWidget)
	{
		UpgradeSelectWidget->RefreshData();
	}
}

void UMortisMainMenuRootWidget::CacheWidgets()
{
	RootSwitcher = FindWidgetByName<UWidgetSwitcher>(this, TEXT("WidgetSwitcher"));
	MainMenuWidget = FindWidgetByName<UUserWidget>(this, TEXT("WBP_MainMenu"));
	UpgradeSelectWidget = FindWidgetByName<UMortisUpgradeSelectWidget>(this, TEXT("WBP_UpgradeSelect"));
	OptionsMenuWidget = FindWidgetByName<UMortisOptionsMenuWidget>(this, TEXT("WBP_OptionsMenu"));

	BTN_StartGame = FindWidgetByName<UButton>(MainMenuWidget, TEXT("BTN_StartGame"));
	BTN_Options = FindWidgetByName<UButton>(MainMenuWidget, TEXT("BTN_Options"));
	BTN_Quit = FindWidgetByName<UButton>(MainMenuWidget, TEXT("BTN_Quit"));
}

void UMortisMainMenuRootWidget::BindWidgetEvents()
{
	if (BTN_StartGame)
	{
		BTN_StartGame->OnClicked.RemoveDynamic(this, &ThisClass::HandleStartClicked);
		BTN_StartGame->OnClicked.AddDynamic(this, &ThisClass::HandleStartClicked);
	}

	if (BTN_Options)
	{
		BTN_Options->OnClicked.RemoveDynamic(this, &ThisClass::HandleOptionsClicked);
		BTN_Options->OnClicked.AddDynamic(this, &ThisClass::HandleOptionsClicked);
	}

	if (BTN_Quit)
	{
		BTN_Quit->OnClicked.RemoveDynamic(this, &ThisClass::HandleQuitClicked);
		BTN_Quit->OnClicked.AddDynamic(this, &ThisClass::HandleQuitClicked);
	}

	if (OptionsMenuWidget)
	{
		OptionsMenuWidget->OnBackRequested.RemoveDynamic(this, &ThisClass::HandleOptionsBackRequested);
		OptionsMenuWidget->OnBackRequested.AddDynamic(this, &ThisClass::HandleOptionsBackRequested);

		OptionsMenuWidget->OnForgetMemoriesApplied.RemoveDynamic(this, &ThisClass::HandleForgetMemoriesApplied);
		OptionsMenuWidget->OnForgetMemoriesApplied.AddDynamic(this, &ThisClass::HandleForgetMemoriesApplied);
	}
}

void UMortisMainMenuRootWidget::UnbindWidgetEvents()
{
	if (BTN_StartGame)
	{
		BTN_StartGame->OnClicked.RemoveDynamic(this, &ThisClass::HandleStartClicked);
	}

	if (BTN_Options)
	{
		BTN_Options->OnClicked.RemoveDynamic(this, &ThisClass::HandleOptionsClicked);
	}

	if (BTN_Quit)
	{
		BTN_Quit->OnClicked.RemoveDynamic(this, &ThisClass::HandleQuitClicked);
	}

	if (OptionsMenuWidget)
	{
		OptionsMenuWidget->OnBackRequested.RemoveDynamic(this, &ThisClass::HandleOptionsBackRequested);
		OptionsMenuWidget->OnForgetMemoriesApplied.RemoveDynamic(this, &ThisClass::HandleForgetMemoriesApplied);
	}
}

void UMortisMainMenuRootWidget::ShowMainMenu()
{
	ShowScreen(TEXT("Screen_MainMenu"), 0);
}

void UMortisMainMenuRootWidget::ShowUpgradeSelect()
{
	if (UpgradeSelectWidget)
	{
		UpgradeSelectWidget->RefreshData();
	}

	ShowScreen(TEXT("Screen_UpgradeSelect"), 1);

	if (UpgradeSelectWidget)
	{
		UpgradeSelectWidget->RefreshFocus();
	}
}

void UMortisMainMenuRootWidget::ShowOptions()
{
	if (OptionsMenuWidget)
	{
		OptionsMenuWidget->RefreshFromSavedSettings();
	}

	ShowScreen(TEXT("Screen_Options"), 2);
}

void UMortisMainMenuRootWidget::ShowScreen(FName ScreenName, int32 FallbackIndex)
{
	if (!RootSwitcher)
	{
		return;
	}

	if (UWidget* TargetScreen = FindWidgetByName<UWidget>(this, ScreenName))
	{
		RootSwitcher->SetActiveWidget(TargetScreen);
		return;
	}

	if (RootSwitcher->GetChildrenCount() > FallbackIndex)
	{
		RootSwitcher->SetActiveWidgetIndex(FallbackIndex);
	}
}

void UMortisMainMenuRootWidget::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
