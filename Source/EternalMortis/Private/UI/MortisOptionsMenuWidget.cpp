#include "UI/MortisOptionsMenuWidget.h"

#include "AudioDevice.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ComboBoxString.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/GameUserSettings.h"
#include "Misc/ConfigCacheIni.h"
#include "Styling/SlateTypes.h"
#include "System/MortisMetaProgressionSubsystem.h"
#include "System/MortisRunStateSubsystem.h"

namespace
{
	constexpr float DefaultMasterVolume = 0.8f;
	const TCHAR* OptionsConfigSection = TEXT("MortisOptions");

	const TArray<FIntPoint>& GetSupportedResolutions()
	{
		static const TArray<FIntPoint> Resolutions = {
			FIntPoint(1280, 720),
			FIntPoint(1600, 900),
			FIntPoint(1920, 1080),
			FIntPoint(2560, 1440),
			FIntPoint(3840, 2160)
		};
		return Resolutions;
	}

	const TArray<FString>& GetRefreshModeOptions()
	{
		static const TArray<FString> Options = {
			TEXT("VSync"),
			TEXT("60 Hz"),
			TEXT("120 Hz"),
			TEXT("144 Hz"),
			TEXT("Unlimited")
		};
		return Options;
	}

	FLinearColor HexColor(uint8 R, uint8 G, uint8 B, float A = 1.0f)
	{
		return FLinearColor(R / 255.0f, G / 255.0f, B / 255.0f, A);
	}

	UObject* GetMenuFont()
	{
		static UObject* FontObject = LoadObject<UObject>(nullptr, TEXT("/Engine/EngineFonts/Roboto.Roboto"));
		return FontObject;
	}

	void SetTextStyle(UTextBlock* Text, int32 Size, const FLinearColor& Color, bool bBold = false)
	{
		if (!Text)
		{
			return;
		}

		FSlateFontInfo Font = Text->GetFont();
		if (UObject* FontObject = GetMenuFont())
		{
			Font.FontObject = FontObject;
		}
		Font.Size = Size;
		Font.TypefaceFontName = bBold ? TEXT("Bold") : TEXT("Regular");
		Text->SetFont(Font);
		Text->SetColorAndOpacity(FSlateColor(Color));
		Text->SetShadowOffset(FVector2D(1.0f, 1.0f));
		Text->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.85f));
	}

	UTextBlock* MakeText(UWidgetTree* Tree, FName Name, const FText& Text, int32 Size, const FLinearColor& Color, bool bBold = false)
	{
		UTextBlock* TextBlock = Tree ? Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name) : nullptr;
		if (TextBlock)
		{
			TextBlock->SetText(Text);
			SetTextStyle(TextBlock, Size, Color, bBold);
		}
		return TextBlock;
	}

	FSlateBrush MakeTintBrush(const FLinearColor& Color)
	{
		FSlateBrush Brush;
		Brush.DrawAs = ESlateBrushDrawType::Box;
		Brush.TintColor = FSlateColor(Color);
		return Brush;
	}

	void StyleComboBox(UComboBoxString* ComboBox)
	{
		if (!ComboBox)
		{
			return;
		}

		const FLinearColor ComboNormal = HexColor(66, 61, 51);
		const FLinearColor ComboHovered = HexColor(84, 78, 65);
		const FLinearColor ComboPressed = HexColor(46, 42, 35);
		const FLinearColor PopupEven = HexColor(31, 29, 25);
		const FLinearColor PopupOdd = HexColor(38, 35, 30);
		const FSlateColor TextColor(HexColor(243, 234, 216));

		FComboBoxStyle ComboStyle = ComboBox->GetWidgetStyle();
		FComboButtonStyle ComboButtonStyle = ComboStyle.ComboButtonStyle;
		FButtonStyle ButtonStyle = ComboButtonStyle.ButtonStyle;
		ButtonStyle
			.SetNormal(MakeTintBrush(ComboNormal))
			.SetHovered(MakeTintBrush(ComboHovered))
			.SetPressed(MakeTintBrush(ComboPressed))
			.SetDisabled(MakeTintBrush(ComboPressed))
			.SetNormalForeground(TextColor)
			.SetHoveredForeground(TextColor)
			.SetPressedForeground(TextColor)
			.SetDisabledForeground(TextColor)
			.SetNormalPadding(FMargin(14.0f, 6.0f, 8.0f, 6.0f))
			.SetPressedPadding(FMargin(14.0f, 7.0f, 8.0f, 5.0f));
		ComboButtonStyle
			.SetButtonStyle(ButtonStyle)
			.SetMenuBorderBrush(MakeTintBrush(HexColor(20, 18, 16, 0.98f)))
			.SetMenuBorderPadding(FMargin(1.0f));
		ComboStyle
			.SetComboButtonStyle(ComboButtonStyle)
			.SetContentPadding(FMargin(14.0f, 6.0f, 8.0f, 6.0f))
			.SetMenuRowPadding(FMargin(10.0f, 5.0f));
		ComboBox->SetWidgetStyle(ComboStyle);

		FTableRowStyle ItemStyle = ComboBox->GetItemStyle();
		ItemStyle
			.SetTextColor(TextColor)
			.SetSelectedTextColor(TextColor)
			.SetActiveBrush(MakeTintBrush(ComboNormal))
			.SetActiveHoveredBrush(MakeTintBrush(ComboHovered))
			.SetInactiveBrush(MakeTintBrush(ComboNormal))
			.SetInactiveHoveredBrush(MakeTintBrush(ComboHovered))
			.SetEvenRowBackgroundBrush(MakeTintBrush(PopupEven))
			.SetOddRowBackgroundBrush(MakeTintBrush(PopupOdd))
			.SetEvenRowBackgroundHoveredBrush(MakeTintBrush(ComboHovered))
			.SetOddRowBackgroundHoveredBrush(MakeTintBrush(ComboHovered));
		ComboBox->SetItemStyle(ItemStyle);
		ComboBox->SetContentPadding(FMargin(14.0f, 6.0f, 8.0f, 6.0f));
		ComboBox->SetMaxListHeight(240.0f);
	}

	void EnableTextWrap(UTextBlock* Text, float WrapAt)
	{
		if (!Text)
		{
			return;
		}

		Text->SetAutoWrapText(true);
		Text->SetWrapTextAt(WrapAt);
	}

	UButton* MakeTextButton(UWidgetTree* Tree, FName ButtonName, FName TextName, const FText& Text, const FVector2D& Size)
	{
		USizeBox* SizeBox = Tree ? Tree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(*(ButtonName.ToString() + TEXT("_SizeBox")))) : nullptr;
		UButton* Button = Tree ? Tree->ConstructWidget<UButton>(UButton::StaticClass(), ButtonName) : nullptr;
		UTextBlock* Label = MakeText(Tree, TextName, Text, 17, HexColor(243, 234, 216), true);
		if (!SizeBox || !Button || !Label)
		{
			return Button;
		}

		SizeBox->SetWidthOverride(Size.X);
		SizeBox->SetHeightOverride(Size.Y);
		Button->AddChild(Label);
		SizeBox->AddChild(Button);
		Label->SetJustification(ETextJustify::Center);
		return Button;
	}

	FSlateChildSize FillSize(float Value = 1.0f)
	{
		FSlateChildSize Size;
		Size.SizeRule = ESlateSizeRule::Fill;
		Size.Value = Value;
		return Size;
	}
}

bool UMortisOptionsMenuWidget::Initialize()
{
	const bool bSuperInitialized = Super::Initialize();

	BuildFallbackWidgetTreeIfNeeded();

	return bSuperInitialized;
}

void UMortisOptionsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CacheWidgets();
	StyleComboBox(Combo_Resolution);
	StyleComboBox(Combo_RefreshMode);
	PopulateOptionLists();
	BindWidgetEvents();
	RefreshFromSavedSettings();
	ShowResetConfirmation(false);
}

void UMortisOptionsMenuWidget::NativeDestruct()
{
	UnbindWidgetEvents();

	Super::NativeDestruct();
}

void UMortisOptionsMenuWidget::RefreshFromSavedSettings()
{
	PendingMasterVolume = DefaultMasterVolume;
	if (GConfig)
	{
		GConfig->GetFloat(OptionsConfigSection, TEXT("MasterVolume"), PendingMasterVolume, GGameUserSettingsIni);
	}
	PendingMasterVolume = FMath::Clamp(PendingMasterVolume, 0.0f, 1.0f);

	if (Slider_MasterVolume)
	{
		Slider_MasterVolume->SetValue(PendingMasterVolume);
	}
	ApplyMasterVolume(PendingMasterVolume);
	UpdateMasterVolumeText();

	UGameUserSettings* GameUserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr;
	const FIntPoint CurrentResolution = GameUserSettings ? GameUserSettings->GetScreenResolution() : FIntPoint(1920, 1080);
	FString ResolutionOption = ResolutionToString(CurrentResolution);
	if (!GetSupportedResolutions().Contains(CurrentResolution))
	{
		ResolutionOption = ResolutionToString(FIntPoint(1920, 1080));
	}

	const FString RefreshModeOption = GameUserSettings
		? RefreshLimitToOption(GameUserSettings->IsVSyncEnabled(), GameUserSettings->GetFrameRateLimit())
		: FString(TEXT("VSync"));

	if (Combo_Resolution)
	{
		Combo_Resolution->SetSelectedOption(ResolutionOption);
	}
	if (Combo_RefreshMode)
	{
		Combo_RefreshMode->SetSelectedOption(RefreshModeOption);
	}

	PendingResolutionOption = ResolutionOption;
	PendingRefreshModeOption = RefreshModeOption;
}

void UMortisOptionsMenuWidget::HandleBackClicked()
{
	ShowResetConfirmation(false);
	OnBackRequested.Broadcast();
}

void UMortisOptionsMenuWidget::HandleApplyClicked()
{
	ApplyPendingVideoSettings();
	SaveMasterVolume();
}

void UMortisOptionsMenuWidget::HandleForgetMemoriesClicked()
{
	ShowResetConfirmation(true);
}

void UMortisOptionsMenuWidget::HandleResetCancelClicked()
{
	ShowResetConfirmation(false);
	if (BTN_ForgetMemories)
	{
		BTN_ForgetMemories->SetKeyboardFocus();
	}
}

void UMortisOptionsMenuWidget::HandleResetConfirmClicked()
{
	ResetProgression();
	ShowResetConfirmation(false);
	OnForgetMemoriesApplied.Broadcast();
	if (BTN_ForgetMemories)
	{
		BTN_ForgetMemories->SetKeyboardFocus();
	}
}

void UMortisOptionsMenuWidget::HandleMasterVolumeChanged(float NewValue)
{
	PendingMasterVolume = FMath::Clamp(NewValue, 0.0f, 1.0f);
	ApplyMasterVolume(PendingMasterVolume);
	UpdateMasterVolumeText();
}

void UMortisOptionsMenuWidget::HandleResolutionSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	PendingResolutionOption = SelectedItem;
}

void UMortisOptionsMenuWidget::HandleRefreshModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	PendingRefreshModeOption = SelectedItem;
}

void UMortisOptionsMenuWidget::BuildFallbackWidgetTreeIfNeeded()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas_OptionsRoot"));
	WidgetTree->RootWidget = RootCanvas;

	UBorder* PanelBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BD_OptionsPanel"));
	PanelBorder->SetBrushColor(HexColor(18, 16, 14, 0.92f));
	PanelBorder->SetPadding(FMargin(34.0f));
	UCanvasPanelSlot* PanelSlot = RootCanvas->AddChildToCanvas(PanelBorder);
	PanelSlot->SetAnchors(FAnchors(0.5f, 0.5f));
	PanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	PanelSlot->SetSize(FVector2D(760.0f, 700.0f));

	UVerticalBox* PanelVBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VB_OptionsPanel"));
	PanelBorder->SetContent(PanelVBox);

	UHorizontalBox* HeaderBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HB_Header"));
	UVerticalBoxSlot* HeaderSlot = PanelVBox->AddChildToVerticalBox(HeaderBox);
	HeaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 22.0f));

	UVerticalBox* HeaderLeft = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VB_HeaderLeft"));
	UHorizontalBoxSlot* HeaderLeftSlot = HeaderBox->AddChildToHorizontalBox(HeaderLeft);
	HeaderLeftSlot->SetSize(FillSize(1.0f));
	HeaderLeftSlot->SetVerticalAlignment(VAlign_Bottom);

	HeaderLeft->AddChildToVerticalBox(MakeText(WidgetTree, TEXT("TXT_OptionsTitle"), NSLOCTEXT("MortisOptions", "OptionsTitle", "Options"), 42, HexColor(243, 234, 216), true));
	HeaderLeft->AddChildToVerticalBox(MakeText(WidgetTree, TEXT("TXT_OptionsSubtitle"), NSLOCTEXT("MortisOptions", "OptionsSubtitle", "Display and audio"), 15, HexColor(185, 170, 146)));

	UVerticalBox* SettingsList = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VB_SettingsList"));
	PanelVBox->AddChildToVerticalBox(SettingsList);

	auto AddSettingRow = [this, SettingsList](FName RowName, const FText& Label, UWidget* Control, bool bDanger)
	{
		USizeBox* RowSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(*(RowName.ToString() + TEXT("_SizeBox"))));
		RowSize->SetHeightOverride(96.0f);
		UVerticalBoxSlot* RowSizeSlot = SettingsList->AddChildToVerticalBox(RowSize);
		RowSizeSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 18.0f));

		UBorder* RowBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), RowName);
		RowBorder->SetPadding(FMargin(18.0f));
		RowBorder->SetBrushColor(bDanger ? HexColor(55, 28, 24, 0.62f) : HexColor(38, 34, 28, 0.84f));
		RowSize->AddChild(RowBorder);

		UHorizontalBox* RowBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(*(RowName.ToString() + TEXT("_Content"))));
		RowBorder->SetContent(RowBox);

		UVerticalBox* LabelBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(*(RowName.ToString() + TEXT("_LabelBox"))));
		USizeBox* LabelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(*(RowName.ToString() + TEXT("_LabelSize"))));
		LabelSize->SetWidthOverride(bDanger ? 384.0f : 210.0f);
		LabelSize->AddChild(LabelBox);
		UHorizontalBoxSlot* LabelSlot = RowBox->AddChildToHorizontalBox(LabelSize);
		LabelSlot->SetVerticalAlignment(VAlign_Center);
		LabelSlot->SetPadding(FMargin(0.0f, 0.0f, 24.0f, 0.0f));

		LabelBox->AddChildToVerticalBox(MakeText(WidgetTree, FName(*(RowName.ToString() + TEXT("_Title"))), Label, 18, HexColor(243, 234, 216), true));

		UHorizontalBoxSlot* ControlSlot = RowBox->AddChildToHorizontalBox(Control);
		if (bDanger)
		{
			ControlSlot->SetHorizontalAlignment(HAlign_Right);
			ControlSlot->SetPadding(FMargin(0.0f, 0.0f, 24.0f, 0.0f));
		}
		else
		{
			ControlSlot->SetSize(FillSize(1.0f));
		}
		ControlSlot->SetVerticalAlignment(VAlign_Center);
	};

	UHorizontalBox* VolumeBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HB_MasterVolume"));
	Slider_MasterVolume = WidgetTree->ConstructWidget<USlider>(USlider::StaticClass(), TEXT("Slider_MasterVolume"));
	Slider_MasterVolume->SetMinValue(0.0f);
	Slider_MasterVolume->SetMaxValue(1.0f);
	Slider_MasterVolume->SetStepSize(0.01f);
	UHorizontalBoxSlot* SliderSlot = VolumeBox->AddChildToHorizontalBox(Slider_MasterVolume);
	SliderSlot->SetSize(FillSize(1.0f));
	SliderSlot->SetVerticalAlignment(VAlign_Center);
	SliderSlot->SetPadding(FMargin(0.0f, 0.0f, 18.0f, 0.0f));

	UBorder* VolumeValueBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BD_MasterVolumeValue"));
	VolumeValueBorder->SetPadding(FMargin(8.0f));
	VolumeValueBorder->SetBrushColor(HexColor(0, 0, 0, 0.28f));
	TXT_MasterVolumeValue = MakeText(WidgetTree, TEXT("TXT_MasterVolumeValue"), FText::FromString(TEXT("80%")), 16, HexColor(241, 207, 130), true);
	TXT_MasterVolumeValue->SetJustification(ETextJustify::Center);
	VolumeValueBorder->SetContent(TXT_MasterVolumeValue);
	USizeBox* VolumeValueSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("SB_MasterVolumeValue"));
	VolumeValueSize->SetWidthOverride(62.0f);
	VolumeValueSize->AddChild(VolumeValueBorder);
	VolumeBox->AddChildToHorizontalBox(VolumeValueSize);

	AddSettingRow(
		TEXT("BD_MasterVolumeRow"),
		NSLOCTEXT("MortisOptions", "MasterVolumeLabel", "Master Volume"),
		VolumeBox,
		false
	);

	Combo_Resolution = WidgetTree->ConstructWidget<UComboBoxString>(UComboBoxString::StaticClass(), TEXT("Combo_Resolution"));
	AddSettingRow(
		TEXT("BD_ResolutionRow"),
		NSLOCTEXT("MortisOptions", "ResolutionLabel", "Resolution"),
		Combo_Resolution,
		false
	);

	UVerticalBox* RefreshBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VB_RefreshMode"));
	Combo_RefreshMode = WidgetTree->ConstructWidget<UComboBoxString>(UComboBoxString::StaticClass(), TEXT("Combo_RefreshMode"));
	RefreshBox->AddChildToVerticalBox(Combo_RefreshMode);
	AddSettingRow(
		TEXT("BD_RefreshModeRow"),
		NSLOCTEXT("MortisOptions", "RefreshModeLabel", "Refresh Mode"),
		RefreshBox,
		false
	);

	BTN_ForgetMemories = MakeTextButton(
		WidgetTree,
		TEXT("BTN_ForgetMemories"),
		TEXT("TXT_ForgetMemoriesButton"),
		NSLOCTEXT("MortisOptions", "ForgetMemoriesButton", "Forget Memories"),
		FVector2D(220.0f, 54.0f)
	);
	AddSettingRow(
		TEXT("BD_ResetRow"),
		NSLOCTEXT("MortisOptions", "ForgetMemoriesLabel", "Forget Memories"),
		BTN_ForgetMemories ? BTN_ForgetMemories->GetParent() : nullptr,
		true
	);

	UHorizontalBox* ButtonRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HB_ActionButtons"));
	UVerticalBoxSlot* ButtonRowSlot = PanelVBox->AddChildToVerticalBox(ButtonRow);
	ButtonRowSlot->SetPadding(FMargin(0.0f, 26.0f, 0.0f, 0.0f));
	ButtonRowSlot->SetHorizontalAlignment(HAlign_Right);

	BTN_Back = MakeTextButton(WidgetTree, TEXT("BTN_Back"), TEXT("TXT_BackButton"), NSLOCTEXT("MortisOptions", "BackButton", "Back"), FVector2D(150.0f, 54.0f));
	UHorizontalBoxSlot* BackSlot = ButtonRow->AddChildToHorizontalBox(BTN_Back ? BTN_Back->GetParent() : nullptr);
	BackSlot->SetPadding(FMargin(0.0f, 0.0f, 14.0f, 0.0f));

	BTN_Apply = MakeTextButton(WidgetTree, TEXT("BTN_Apply"), TEXT("TXT_ApplyButton"), NSLOCTEXT("MortisOptions", "ApplyButton", "Apply"), FVector2D(150.0f, 54.0f));
	ButtonRow->AddChildToHorizontalBox(BTN_Apply ? BTN_Apply->GetParent() : nullptr);

	BD_ResetModalLayer = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BD_ResetModalLayer"));
	BD_ResetModalLayer->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.68f));
	BD_ResetModalLayer->SetPadding(FMargin(40.0f));
	BD_ResetModalLayer->SetVisibility(ESlateVisibility::Collapsed);
	UCanvasPanelSlot* ModalLayerSlot = RootCanvas->AddChildToCanvas(BD_ResetModalLayer);
	ModalLayerSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	ModalLayerSlot->SetOffsets(FMargin(0.0f));

	UOverlay* ModalOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("OV_ResetModal"));
	BD_ResetModalLayer->SetContent(ModalOverlay);

	UBorder* ConfirmCard = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BD_ResetConfirmCard"));
	ConfirmCard->SetBrushColor(HexColor(20, 16, 14, 0.98f));
	ConfirmCard->SetPadding(FMargin(28.0f));
	USizeBox* ConfirmCardSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("SB_ResetConfirmCard"));
	ConfirmCardSize->SetWidthOverride(520.0f);
	ConfirmCardSize->AddChild(ConfirmCard);
	UOverlaySlot* ConfirmCardSlot = ModalOverlay->AddChildToOverlay(ConfirmCardSize);
	ConfirmCardSlot->SetHorizontalAlignment(HAlign_Center);
	ConfirmCardSlot->SetVerticalAlignment(VAlign_Center);

	UVerticalBox* ConfirmVBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VB_ResetConfirmCard"));
	ConfirmCard->SetContent(ConfirmVBox);

	ConfirmVBox->AddChildToVerticalBox(MakeText(WidgetTree, TEXT("TXT_ResetTitle"), NSLOCTEXT("MortisOptions", "ResetTitle", "Forget Memories?"), 32, HexColor(223, 106, 94), true));
	UTextBlock* ResetBody = MakeText(
		WidgetTree,
		TEXT("TXT_ResetBody"),
		NSLOCTEXT("MortisOptions", "ResetBody", "This cannot be undone in the current save. The save slot is rewritten with reset values."),
		16,
		HexColor(185, 170, 146)
	);
	EnableTextWrap(ResetBody, 460.0f);
	UVerticalBoxSlot* ResetBodySlot = ConfirmVBox->AddChildToVerticalBox(ResetBody);
	ResetBodySlot->SetPadding(FMargin(0.0f, 12.0f, 0.0f, 20.0f));

	UBorder* ConfirmListBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BD_ResetList"));
	ConfirmListBorder->SetPadding(FMargin(16.0f));
	ConfirmListBorder->SetBrushColor(HexColor(0, 0, 0, 0.28f));
	UVerticalBoxSlot* ConfirmListSlot = ConfirmVBox->AddChildToVerticalBox(ConfirmListBorder);
	ConfirmListSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 24.0f));

	UVerticalBox* ConfirmList = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VB_ResetList"));
	ConfirmListBorder->SetContent(ConfirmList);
	const TArray<FText> ResetLines = {
		NSLOCTEXT("MortisOptions", "ResetLineMemory", "Memory fragments reset to 0"),
		NSLOCTEXT("MortisOptions", "ResetLineUpgrades", "Unlocked upgrades cleared"),
		NSLOCTEXT("MortisOptions", "ResetLineRuneSlots", "Rune slot upgrades reset"),
		NSLOCTEXT("MortisOptions", "ResetLineTutorial", "Tutorial progress reset"),
		NSLOCTEXT("MortisOptions", "ResetLineGold", "Current run gold reset")
	};
	for (int32 Index = 0; Index < ResetLines.Num(); ++Index)
	{
		UVerticalBoxSlot* LineSlot = ConfirmList->AddChildToVerticalBox(MakeText(
			WidgetTree,
			FName(*FString::Printf(TEXT("TXT_ResetLine_%d"), Index)),
			ResetLines[Index],
			15,
			HexColor(243, 234, 216)
		));
		LineSlot->SetPadding(FMargin(0.0f, Index == 0 ? 0.0f : 9.0f, 0.0f, 0.0f));
	}

	UHorizontalBox* ModalButtons = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HB_ResetActions"));
	UVerticalBoxSlot* ModalButtonsSlot = ConfirmVBox->AddChildToVerticalBox(ModalButtons);
	ModalButtonsSlot->SetHorizontalAlignment(HAlign_Right);

	BTN_ResetCancel = MakeTextButton(WidgetTree, TEXT("BTN_ResetCancel"), TEXT("TXT_ResetCancel"), NSLOCTEXT("MortisOptions", "CancelButton", "Cancel"), FVector2D(150.0f, 54.0f));
	UHorizontalBoxSlot* CancelSlot = ModalButtons->AddChildToHorizontalBox(BTN_ResetCancel ? BTN_ResetCancel->GetParent() : nullptr);
	CancelSlot->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));

	BTN_ResetConfirm = MakeTextButton(WidgetTree, TEXT("BTN_ResetConfirm"), TEXT("TXT_ResetConfirm"), NSLOCTEXT("MortisOptions", "ForgetButton", "Forget"), FVector2D(150.0f, 54.0f));
	ModalButtons->AddChildToHorizontalBox(BTN_ResetConfirm ? BTN_ResetConfirm->GetParent() : nullptr);
}

void UMortisOptionsMenuWidget::CacheWidgets()
{
	if (!WidgetTree)
	{
		return;
	}

	Slider_MasterVolume = Cast<USlider>(WidgetTree->FindWidget(TEXT("Slider_MasterVolume")));
	TXT_MasterVolumeValue = Cast<UTextBlock>(WidgetTree->FindWidget(TEXT("TXT_MasterVolumeValue")));
	Combo_Resolution = Cast<UComboBoxString>(WidgetTree->FindWidget(TEXT("Combo_Resolution")));
	Combo_RefreshMode = Cast<UComboBoxString>(WidgetTree->FindWidget(TEXT("Combo_RefreshMode")));
	BTN_ForgetMemories = Cast<UButton>(WidgetTree->FindWidget(TEXT("BTN_ForgetMemories")));
	BTN_Back = Cast<UButton>(WidgetTree->FindWidget(TEXT("BTN_Back")));
	BTN_Apply = Cast<UButton>(WidgetTree->FindWidget(TEXT("BTN_Apply")));
	BTN_ResetCancel = Cast<UButton>(WidgetTree->FindWidget(TEXT("BTN_ResetCancel")));
	BTN_ResetConfirm = Cast<UButton>(WidgetTree->FindWidget(TEXT("BTN_ResetConfirm")));
	BD_ResetModalLayer = Cast<UBorder>(WidgetTree->FindWidget(TEXT("BD_ResetModalLayer")));
}

void UMortisOptionsMenuWidget::BindWidgetEvents()
{
	if (BTN_Back)
	{
		BTN_Back->OnClicked.RemoveDynamic(this, &ThisClass::HandleBackClicked);
		BTN_Back->OnClicked.AddDynamic(this, &ThisClass::HandleBackClicked);
	}
	if (BTN_Apply)
	{
		BTN_Apply->OnClicked.RemoveDynamic(this, &ThisClass::HandleApplyClicked);
		BTN_Apply->OnClicked.AddDynamic(this, &ThisClass::HandleApplyClicked);
	}
	if (BTN_ForgetMemories)
	{
		BTN_ForgetMemories->OnClicked.RemoveDynamic(this, &ThisClass::HandleForgetMemoriesClicked);
		BTN_ForgetMemories->OnClicked.AddDynamic(this, &ThisClass::HandleForgetMemoriesClicked);
	}
	if (BTN_ResetCancel)
	{
		BTN_ResetCancel->OnClicked.RemoveDynamic(this, &ThisClass::HandleResetCancelClicked);
		BTN_ResetCancel->OnClicked.AddDynamic(this, &ThisClass::HandleResetCancelClicked);
	}
	if (BTN_ResetConfirm)
	{
		BTN_ResetConfirm->OnClicked.RemoveDynamic(this, &ThisClass::HandleResetConfirmClicked);
		BTN_ResetConfirm->OnClicked.AddDynamic(this, &ThisClass::HandleResetConfirmClicked);
	}
	if (Slider_MasterVolume)
	{
		Slider_MasterVolume->OnValueChanged.RemoveDynamic(this, &ThisClass::HandleMasterVolumeChanged);
		Slider_MasterVolume->OnValueChanged.AddDynamic(this, &ThisClass::HandleMasterVolumeChanged);
	}
	if (Combo_Resolution)
	{
		Combo_Resolution->OnSelectionChanged.RemoveDynamic(this, &ThisClass::HandleResolutionSelectionChanged);
		Combo_Resolution->OnSelectionChanged.AddDynamic(this, &ThisClass::HandleResolutionSelectionChanged);
	}
	if (Combo_RefreshMode)
	{
		Combo_RefreshMode->OnSelectionChanged.RemoveDynamic(this, &ThisClass::HandleRefreshModeSelectionChanged);
		Combo_RefreshMode->OnSelectionChanged.AddDynamic(this, &ThisClass::HandleRefreshModeSelectionChanged);
	}
}

void UMortisOptionsMenuWidget::UnbindWidgetEvents()
{
	if (BTN_Back)
	{
		BTN_Back->OnClicked.RemoveDynamic(this, &ThisClass::HandleBackClicked);
	}
	if (BTN_Apply)
	{
		BTN_Apply->OnClicked.RemoveDynamic(this, &ThisClass::HandleApplyClicked);
	}
	if (BTN_ForgetMemories)
	{
		BTN_ForgetMemories->OnClicked.RemoveDynamic(this, &ThisClass::HandleForgetMemoriesClicked);
	}
	if (BTN_ResetCancel)
	{
		BTN_ResetCancel->OnClicked.RemoveDynamic(this, &ThisClass::HandleResetCancelClicked);
	}
	if (BTN_ResetConfirm)
	{
		BTN_ResetConfirm->OnClicked.RemoveDynamic(this, &ThisClass::HandleResetConfirmClicked);
	}
	if (Slider_MasterVolume)
	{
		Slider_MasterVolume->OnValueChanged.RemoveDynamic(this, &ThisClass::HandleMasterVolumeChanged);
	}
	if (Combo_Resolution)
	{
		Combo_Resolution->OnSelectionChanged.RemoveDynamic(this, &ThisClass::HandleResolutionSelectionChanged);
	}
	if (Combo_RefreshMode)
	{
		Combo_RefreshMode->OnSelectionChanged.RemoveDynamic(this, &ThisClass::HandleRefreshModeSelectionChanged);
	}
}

void UMortisOptionsMenuWidget::PopulateOptionLists()
{
	if (Combo_Resolution)
	{
		Combo_Resolution->ClearOptions();
		for (const FIntPoint& Resolution : GetSupportedResolutions())
		{
			Combo_Resolution->AddOption(ResolutionToString(Resolution));
		}
	}

	if (Combo_RefreshMode)
	{
		Combo_RefreshMode->ClearOptions();
		for (const FString& Option : GetRefreshModeOptions())
		{
			Combo_RefreshMode->AddOption(Option);
		}
	}
}

void UMortisOptionsMenuWidget::ApplyPendingVideoSettings() const
{
	UGameUserSettings* GameUserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr;
	if (!GameUserSettings)
	{
		return;
	}

	FIntPoint Resolution;
	if (TryParseResolution(PendingResolutionOption, Resolution))
	{
		GameUserSettings->SetScreenResolution(Resolution);
	}

	const bool bUseVSync = PendingRefreshModeOption == TEXT("VSync");
	GameUserSettings->SetVSyncEnabled(bUseVSync);
	GameUserSettings->SetFrameRateLimit(RefreshOptionToFrameRateLimit(PendingRefreshModeOption));
	GameUserSettings->ApplySettings(false);
	GameUserSettings->SaveSettings();
}

void UMortisOptionsMenuWidget::SaveMasterVolume() const
{
	if (GConfig)
	{
		GConfig->SetFloat(OptionsConfigSection, TEXT("MasterVolume"), PendingMasterVolume, GGameUserSettingsIni);
		GConfig->Flush(false, GGameUserSettingsIni);
	}
}

void UMortisOptionsMenuWidget::ApplyMasterVolume(float Volume) const
{
	if (UWorld* World = GetWorld())
	{
		if (FAudioDevice* AudioDevice = World->GetAudioDeviceRaw())
		{
			AudioDevice->SetTransientPrimaryVolume(FMath::Clamp(Volume, 0.0f, 1.0f));
		}
	}
}

void UMortisOptionsMenuWidget::UpdateMasterVolumeText()
{
	if (TXT_MasterVolumeValue)
	{
		const int32 Percent = FMath::RoundToInt(PendingMasterVolume * 100.0f);
		TXT_MasterVolumeValue->SetText(FText::Format(NSLOCTEXT("MortisOptions", "VolumePercentFormat", "{0}%"), FText::AsNumber(Percent)));
	}
}

void UMortisOptionsMenuWidget::ShowResetConfirmation(bool bShow)
{
	if (BD_ResetModalLayer)
	{
		BD_ResetModalLayer->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (bShow && BTN_ResetCancel)
	{
		BTN_ResetCancel->SetKeyboardFocus();
	}
}

void UMortisOptionsMenuWidget::ResetProgression()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	if (UMortisMetaProgressionSubsystem* MetaProgression = GameInstance->GetSubsystem<UMortisMetaProgressionSubsystem>())
	{
		MetaProgression->ResetAllMetaProgression();
	}

	if (UMortisRunStateSubsystem* RunState = GameInstance->GetSubsystem<UMortisRunStateSubsystem>())
	{
		RunState->ResetRunState();
	}
}

FString UMortisOptionsMenuWidget::ResolutionToString(const FIntPoint& Resolution) const
{
	return FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y);
}

bool UMortisOptionsMenuWidget::TryParseResolution(const FString& Option, FIntPoint& OutResolution) const
{
	for (const FIntPoint& Resolution : GetSupportedResolutions())
	{
		if (Option == ResolutionToString(Resolution))
		{
			OutResolution = Resolution;
			return true;
		}
	}

	return false;
}

FString UMortisOptionsMenuWidget::RefreshLimitToOption(bool bVSyncEnabled, float FrameRateLimit) const
{
	if (bVSyncEnabled)
	{
		return TEXT("VSync");
	}

	if (FMath::IsNearlyEqual(FrameRateLimit, 60.0f, 1.0f))
	{
		return TEXT("60 Hz");
	}
	if (FMath::IsNearlyEqual(FrameRateLimit, 120.0f, 1.0f))
	{
		return TEXT("120 Hz");
	}
	if (FMath::IsNearlyEqual(FrameRateLimit, 144.0f, 1.0f))
	{
		return TEXT("144 Hz");
	}

	return TEXT("Unlimited");
}

float UMortisOptionsMenuWidget::RefreshOptionToFrameRateLimit(const FString& Option) const
{
	if (Option == TEXT("60 Hz"))
	{
		return 60.0f;
	}
	if (Option == TEXT("120 Hz"))
	{
		return 120.0f;
	}
	if (Option == TEXT("144 Hz"))
	{
		return 144.0f;
	}

	return 0.0f;
}
