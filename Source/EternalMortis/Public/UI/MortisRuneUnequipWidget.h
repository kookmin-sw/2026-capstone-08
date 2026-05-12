#pragma once

#include "CoreMinimal.h"
#include "Types/MortisRuneDataTypes.h"
#include "UI/MortisRuneSynergyEntryWidget.h"
#include "UI/MortisWidgetBase.h"
#include "MortisRuneUnequipWidget.generated.h"

class UButton;
class UTextBlock;
class UWidget;
class UMortisEquippedRunePanelWidget;
class UMortisRightRuneDetailWidget;
class UMortisRuneCardWidget;
class UMortisRuneDatabaseSubsystem;
class UMortisRuneInventorySubsystem;
class UMortisRunStateSubsystem;

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisRuneUnequipWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mortis|RuneUnequip")
    void RefreshAll();

    UFUNCTION(BlueprintCallable, Category = "Mortis|RuneUnequip")
    void CloseScreen();

    UFUNCTION(BlueprintCallable, Category = "Mortis|RuneUnequip")
    void ResetScreenState();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    UFUNCTION()
    void HandleEquippedRuneSelected(FMortisRuneInstance ClickedRune);

    UFUNCTION()
    void HandleUnequipRequested();

    UFUNCTION()
    void HandleConfirmUnequipClicked();

    UFUNCTION()
    void HandleCancelUnequipClicked();

    UFUNCTION()
    void HandleCloseClicked();

    UFUNCTION()
    void HandleGoldChanged(int32 DeltaGold);

    UFUNCTION()
    void HandleEquippedRuneAdded(int32 SlotIndex, const FMortisRuneInstance& AddedRune);

    UFUNCTION()
    void HandleEquippedRuneRemoved(int32 SlotIndex, const FMortisRuneInstance& RemovedRune);

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UMortisEquippedRunePanelWidget> Widget_EquippedRunePanel = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UMortisRightRuneDetailWidget> Widget_RightRuneDetail = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UWidget> Overlay_ConfirmUnequip = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UButton> Button_ConfirmUnequip = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UButton> Button_CancelUnequip = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UButton> Button_Close = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_ConfirmUnequipMessage = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_ConfirmUnequipCost = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|RuneUnequip", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UMortisRuneCardWidget> RuneCardClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|RuneUnequip", meta = (ClampMin = "0", AllowPrivateAccess = "true"))
    int32 UnequipCost = 100;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|RuneUnequip", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMortisRuneInventorySubsystem> RuneInventorySubsystemRef = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|RuneUnequip", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMortisRuneDatabaseSubsystem> RuneDatabaseSubsystemRef = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|RuneUnequip", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMortisRunStateSubsystem> RunStateSubsystemRef = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|RuneUnequip", meta = (AllowPrivateAccess = "true"))
    FMortisRuneInstance SelectedRune;

private:
    void InitializeReferences();
    void BindDelegates();
    void UnbindDelegates();
    void RefreshEquippedRunePanel();
    void RefreshRightRuneDetail();
    void RefreshConfirmationText();
    void ClearSelectedRune();
    void SetConfirmOverlayVisible(bool bVisible);

    bool HasSelectedRune() const;
    bool IsSelectedRuneStillEquipped() const;
    bool CanUnequipSelectedRune() const;
    int32 FindSelectedRuneSlotIndex() const;
    int32 GetEquippedCountBySetTag(const FGameplayTag& SetTag) const;

    FText GetSelectedRuneNameText() const;
    FText BuildRuneStatValueText() const;
    FText BuildSetProgressTextBySetTag(const FGameplayTag& SetTag) const;
    FText BuildUnequipCostText() const;
    FText BuildCurrentGoldText() const;
    FLinearColor GetGlyphTintBySetTag(const FGameplayTag& SetTag) const;
    void BuildSelectedRuneSynergyEntries(const FGameplayTag& SetTag, TArray<FMortisRuneSynergyEntryViewData>& OutEntries) const;
};
