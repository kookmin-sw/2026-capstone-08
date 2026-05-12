#pragma once

#include "CoreMinimal.h"
#include "UI/MortisWidgetBase.h"
#include "MortisRuneSlotUnlockPanelWidget.generated.h"

class UButton;
class UHorizontalBox;
class UTextBlock;
class UMortisMetaProgressionSubsystem;
class UMortisRuneSlotIconWidget;

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisRuneSlotUnlockPanelWidget : public UMortisWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression|RuneSlot")
	void RefreshAll();

	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression|RuneSlot")
	void RefreshFocus();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleUnlockRuneSlotClicked();

	UFUNCTION()
	void HandleMemoryFragmentsChanged(int32 NewValue);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TXT_RuneSlotTitle = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TXT_RuneSlotCount = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UHorizontalBox> HB_RuneSlotIcons = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TXT_RuneSlotCost = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UButton> BTN_UnlockRuneSlot = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TXT_UnlockRuneSlot = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMortisRuneSlotIconWidget> RuneSlotIconClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot|Text", meta = (AllowPrivateAccess = "true"))
	FText RuneSlotTitleText = NSLOCTEXT("MortisMetaProgression", "RuneSlotTitleText", "Rune Slots");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot|Text", meta = (AllowPrivateAccess = "true"))
	FText RuneSlotTitleWithCountFormatText = NSLOCTEXT("MortisMetaProgression", "RuneSlotTitleWithCountFormatText", "{0} {1} / {2}");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot|Text", meta = (AllowPrivateAccess = "true"))
	FText RuneSlotCountFormatText = NSLOCTEXT("MortisMetaProgression", "RuneSlotCountFormatText", "{0} / {1}");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot|Text", meta = (AllowPrivateAccess = "true"))
	FText UnlockRuneSlotText = NSLOCTEXT("MortisMetaProgression", "UnlockRuneSlotText", "Unlock Slot");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot|Text", meta = (AllowPrivateAccess = "true"))
	FText MaxRuneSlotText = NSLOCTEXT("MortisMetaProgression", "MaxRuneSlotText", "Max slots unlocked");

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMortisMetaProgressionSubsystem> MetaSubsystemRef = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMortisRuneSlotIconWidget>> CreatedSlotIcons;

private:
	void InitializeReferences();
	void BindWidgetEvents();
	void UnbindWidgetEvents();
	void BindMetaDelegates();
	void UnbindMetaDelegates();
	void RebuildSlotIcons();
	TSubclassOf<UMortisRuneSlotIconWidget> ResolveRuneSlotIconClass() const;
	void RefreshSlotIconStates();
	void RefreshTexts();
	void RefreshUnlockButton();
	int32 GetMemoryFragments() const;
	bool GetNextUpgradeCost(int32& OutCost) const;
	bool IsAtMaxRuneSlots() const;
	bool CanAttemptRuneSlotUpgrade() const;
};
