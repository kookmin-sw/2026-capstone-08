#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/MortisRuneDataTypes.h"
#include "UI/MortisRuneSynergyEntryWidget.h"
#include "UI/MortisWidgetBase.h"
#include "MortisInventoryRunePageWidget.generated.h"

class UMortisRuneInventorySubsystem;
class UMortisRuneDatabaseSubsystem;
class UMortisRuneSetEntryWidget;
class UMortisRuneCardWidget;
class UMortisRightRuneDetailWidget;

class UDataTable;
class UScrollBox;
class UUniformGridPanel;
class UWidgetSwitcher;
class UTextBlock;

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisInventoryRunePageWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void RefreshAll();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void HandleSetEntryClicked(FGameplayTag ClickedSetTag);

    UFUNCTION()
    void HandleRuneCardClicked(FMortisRuneInstance ClickedRune);

    UFUNCTION()
    void HandleEquipButtonClicked();

    UFUNCTION()
    void HandleOwningRuneAdded(const FMortisRuneInstance& AddedRune);

    UFUNCTION()
    void HandleOwningRuneRemoved(const FMortisRuneInstance& RemovedRune);

    UFUNCTION()
    void HandleEquippedRuneAdded(int32 SlotIndex, const FMortisRuneInstance& AddedRune);

    UFUNCTION()
    void HandleEquippedRuneRemoved(int32 SlotIndex, const FMortisRuneInstance& RemovedRune);

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UScrollBox> ScrollBox_SetList = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UWidgetSwitcher> WidgetSwitcher_CenterState = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UUniformGridPanel> UniformGrid_EquippedRuneGrid = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UUniformGridPanel> UniformGrid_RuneGrid = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_SelectedSetName = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_RuneCount = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_EmptyMessage = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UWidgetSwitcher> WidgetSwitcher_RightState = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_RightOverviewTitle = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_EquippedSummaryTitle = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_EquippedSummaryBody = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_ActiveSetSummaryTitle = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_ActiveSetSummaryBody = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_RightSetTitle = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_SelectedSetName_Right = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_SelectedSetCount = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_SetEffectTier01 = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_SetEffectTier02 = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_SetEffectTier03 = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UMortisRightRuneDetailWidget> Widget_RightRuneDetail = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UMortisRuneSetEntryWidget> RuneSetEntryClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UMortisRuneCardWidget> RuneCardClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (ClampMin = "1", AllowPrivateAccess = "true"))
    int32 RuneColumns = 4;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (ClampMin = "1", AllowPrivateAccess = "true"))
    int32 EquippedRuneColumns = 3;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FGameplayTag SelectedSetTag;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FMortisRuneInstance SelectedRune;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMortisRuneInventorySubsystem> RuneInventorySubsystemRef = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMortisRuneDatabaseSubsystem> RuneDatabaseSubsystemRef = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UDataTable> RuneSetTableRef = nullptr;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMortisRuneSetEntryWidget>> CreatedSetEntries;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMortisRuneCardWidget>> CreatedRuneCards;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMortisRuneCardWidget>> CreatedEquippedRuneCards;

private:
    void InitializeReferences();
    void BindInventoryDelegates();
    void UnbindInventoryDelegates();

    void RefreshSetList();
    void UpdateCenterState();
    void UpdateCenterHeader();
    void RefreshEquippedRuneGrid();
    void RefreshRuneGrid();
    void RefreshRightPanel();
    void ClearEquippedRuneGrid();
    void ClearRuneGrid();
    void UpdateRightOverview();
    void UpdateRightSetDetail();
    void UpdateRightRuneDetail();
    void BuildSelectedRuneSynergyEntries(const FGameplayTag& SetTag, TArray<FMortisRuneSynergyEntryViewData>& OutEntries) const;

    int32 GetTotalOwnedCountBySetTag(const FGameplayTag& SetTag) const;
    void GetOwningRunesBySetTag(const FGameplayTag& SetTag, TArray<FMortisRuneInstance>& OutRunes) const;
    bool IsRuneEquipped(const FGuid& RuneId) const;
    bool HasSelectedRune() const;
    int32 GetEquippedRuneCount() const;
    int32 GetEquippedCountBySetTag(const FGameplayTag& SetTag) const;
    FLinearColor GetGlyphTintBySetTag(const FGameplayTag& SetTag) const;
    int32 FindFirstEmptyRuneSlot() const;
    bool CanEquipSelectedRune() const;
    FText BuildEquippedSummaryText() const;
    FText BuildActiveSetSummaryText() const;
    FText BuildSelectedSetCountText() const;
    FText BuildSetProgressTextBySetTag(const FGameplayTag& SetTag) const;
    FText BuildRuneStatValueText() const;
    FText GetSelectedRuneNameText() const;
    FText GetEquipButtonLabelText() const;
};
