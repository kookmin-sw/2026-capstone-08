#pragma once

#include "CoreMinimal.h"
#include "Types/MortisRuneDataTypes.h"
#include "UI/MortisWidgetBase.h"
#include "MortisEquippedRunePanelWidget.generated.h"

class UMortisRuneCardWidget;
class UMortisRuneDatabaseSubsystem;
class UMortisRuneInventorySubsystem;
class UUniformGridPanel;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortisOnEquippedRuneSelected, FMortisRuneInstance, SelectedRune);

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisEquippedRunePanelWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void InitializeEquippedRunePanel(
        UMortisRuneInventorySubsystem* InRuneInventorySubsystem,
        UMortisRuneDatabaseSubsystem* InRuneDatabaseSubsystem,
        TSubclassOf<UMortisRuneCardWidget> InRuneCardClass);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void RefreshEquippedRunes();

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void SetSelectedRuneState(const FMortisRuneInstance& InSelectedRune, bool bInSelectedFromEquipped);

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void ClearSelectedRuneState();

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Rune")
    void SetRuneCardClass(TSubclassOf<UMortisRuneCardWidget> InRuneCardClass);

    UPROPERTY(BlueprintAssignable, Category = "Mortis|Inventory|Rune")
    FMortisOnEquippedRuneSelected OnEquippedRuneSelected;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void HandleEquippedRuneCardClicked(FMortisRuneInstance ClickedRune);

    UFUNCTION()
    void HandleEquippedRuneAdded(int32 SlotIndex, const FMortisRuneInstance& AddedRune);

    UFUNCTION()
    void HandleEquippedRuneRemoved(int32 SlotIndex, const FMortisRuneInstance& RemovedRune);

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UUniformGridPanel> UniformGrid_EquippedRuneGrid = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UMortisRuneCardWidget> RuneCardClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Layout", meta = (ClampMin = "1", AllowPrivateAccess = "true"))
    int32 EquippedRuneColumns = 5;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Layout", meta = (ClampMin = "1", AllowPrivateAccess = "true"))
    int32 MaxDisplayedEquippedRuneSlots = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Rune|Layout", meta = (ClampMin = "1", AllowPrivateAccess = "true"))
    FVector2D EquippedRuneCardSize = FVector2D(48.0f, 48.0f);

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMortisRuneInventorySubsystem> RuneInventorySubsystemRef = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMortisRuneDatabaseSubsystem> RuneDatabaseSubsystemRef = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    FMortisRuneInstance SelectedRune;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|Inventory|Rune", meta = (AllowPrivateAccess = "true"))
    bool bSelectedFromEquipped = false;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMortisRuneCardWidget>> CreatedRuneCards;

private:
    void InitializeReferences();
    void BindInventoryDelegates();
    void UnbindInventoryDelegates();
    void ClearEquippedRuneGrid();
    void ConfigureRuneSlotCard(UMortisRuneCardWidget* RuneCard, const FMortisRuneInstance& RuneInstance, bool bUnlocked);
    FLinearColor GetGlyphTintBySetTag(const FGameplayTag& SetTag) const;
    bool IsSelectedEquippedRuneStillValid() const;
};
