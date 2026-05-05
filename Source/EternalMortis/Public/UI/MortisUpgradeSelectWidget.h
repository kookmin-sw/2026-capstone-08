#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/MortisMetaProgressionDataTypes.h"
#include "UI/MortisWidgetBase.h"
#include "MortisUpgradeSelectWidget.generated.h"

class UButton;
class UDataTable;
class UHorizontalBox;
class UImage;
class UTextBlock;
class UTexture2D;
class UMortisMetaProgressionSubsystem;
class UMortisUpgradeIconEntryWidget;

USTRUCT(BlueprintType)
struct FMortisUpgradeSelectEntryData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|MetaProgression")
	FGameplayTag ExperienceTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|MetaProgression")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|MetaProgression")
	FText Description;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|MetaProgression")
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|MetaProgression")
	int32 UnlockCost = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Mortis|MetaProgression")
	bool bNoUpgradeEntry = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMortisOnRunStartRequested, FGameplayTag, SelectedExperienceTag, bool, bNoUpgrade);

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisUpgradeSelectWidget : public UMortisWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression")
	void RefreshData();

	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression")
	void RefreshFocus();

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression")
	bool GetActiveEntryData(FMortisUpgradeSelectEntryData& OutEntryData) const;

	UPROPERTY(BlueprintAssignable, Category = "Mortis|MetaProgression")
	FMortisOnRunStartRequested OnRunStartRequested;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleEntryClicked(FGameplayTag ClickedExperienceTag, bool bNoUpgradeEntry);

	UFUNCTION()
	void HandleActionClicked();

	UFUNCTION()
	void HandleMemoryFragmentsChanged(int32 NewValue);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UImage> IMG_CharacterPreview = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UImage> IMG_SelectedIcon = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TXT_SelectedName = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TXT_SelectedState = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TXT_SelectedDescription = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UHorizontalBox> HB_Cost = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TXT_CostLabel = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TXT_CostValue = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UButton> BTN_Action = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TXT_Action = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UHorizontalBox> HB_UpgradeEntries = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMortisUpgradeIconEntryWidget> EntryWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|No Upgrade", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTexture2D> NoUpgradeIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|No Upgrade", meta = (AllowPrivateAccess = "true"))
	FText NoUpgradeDisplayName = NSLOCTEXT("MortisMetaProgression", "NoUpgradeDisplayName", "No Upgrade");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|No Upgrade", meta = (AllowPrivateAccess = "true", MultiLine = "true"))
	FText NoUpgradeDescription = NSLOCTEXT("MortisMetaProgression", "NoUpgradeDescription", "Start the run without an upgrade.");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|Text", meta = (AllowPrivateAccess = "true"))
	FText StartActionText = NSLOCTEXT("MortisMetaProgression", "StartActionText", "Start Game");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|Text", meta = (AllowPrivateAccess = "true"))
	FText UnlockActionText = NSLOCTEXT("MortisMetaProgression", "UnlockActionText", "Unlock");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|Text", meta = (AllowPrivateAccess = "true"))
	FText NoUpgradeStateText = NSLOCTEXT("MortisMetaProgression", "NoUpgradeStateText", "Available");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|Text", meta = (AllowPrivateAccess = "true"))
	FText UnlockedStateText = NSLOCTEXT("MortisMetaProgression", "UnlockedStateText", "Unlocked");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|Text", meta = (AllowPrivateAccess = "true"))
	FText LockedStateText = NSLOCTEXT("MortisMetaProgression", "LockedStateText", "Locked");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|Text", meta = (AllowPrivateAccess = "true"))
	FText InsufficientFragmentsStateText = NSLOCTEXT("MortisMetaProgression", "InsufficientFragmentsStateText", "Not enough fragments");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|Text", meta = (AllowPrivateAccess = "true"))
	FText CostLabelText = NSLOCTEXT("MortisMetaProgression", "CostLabelText", "Required Fragments");

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|MetaProgression", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMortisMetaProgressionSubsystem> MetaSubsystemRef = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> ExperienceTableRef = nullptr;

	UPROPERTY(Transient)
	TArray<FMortisUpgradeSelectEntryData> CachedEntries;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMortisUpgradeIconEntryWidget>> CreatedEntryWidgets;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|MetaProgression", meta = (AllowPrivateAccess = "true"))
	FMortisUpgradeSelectEntryData ActiveEntryData;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|MetaProgression", meta = (AllowPrivateAccess = "true"))
	bool bHasActiveEntry = false;

private:
	void InitializeReferences();
	void BindWidgetEvents();
	void UnbindWidgetEvents();
	void BindMetaDelegates();
	void UnbindMetaDelegates();
	void RebuildEntryList();
	void AddNoUpgradeEntryData();
	void AddExperienceEntryData();
	UMortisUpgradeIconEntryWidget* CreateEntryWidget(const FMortisUpgradeSelectEntryData& EntryData);
	void SelectDefaultEntry();
	bool SetActiveEntry(FGameplayTag ExperienceTag, bool bNoUpgradeEntry);
	bool FindEntryData(FGameplayTag ExperienceTag, bool bNoUpgradeEntry, FMortisUpgradeSelectEntryData& OutEntryData) const;
	bool IsEntryActive(const FMortisUpgradeSelectEntryData& EntryData) const;
	bool IsEntryUnlocked(const FMortisUpgradeSelectEntryData& EntryData) const;
	bool CanUnlockEntry(const FMortisUpgradeSelectEntryData& EntryData) const;
	int32 GetMemoryFragments() const;
	void RefreshEntryStates();
	void RefreshDetailPanel();
	void RefreshActionButton();
	bool TryUnlockActiveEntry();
	void StartRunWithActiveEntry();
	void ClearDetailPanel();
};
