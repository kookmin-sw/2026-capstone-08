#pragma once

#include "CoreMinimal.h"
#include "Types/MortisCurseDataTypes.h"
#include "UI/MortisCurseDetailWidget.h"
#include "UI/MortisCurseEntryWidget.h"
#include "UI/MortisWidgetBase.h"
#include "MortisInventoryCursePageWidget.generated.h"

class UButton;
class UCanvasPanel;
class UTextBlock;
class UTexture2D;
class UWidget;
class UMortisCurseDatabaseSubsystem;
class UMortisCurseInventorySubsystem;

USTRUCT()
struct FMortisCurseEntryRuntimeData
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UMortisCurseEntryWidget> EntryWidget = nullptr;

    UPROPERTY()
    FMortisCurseInstance Curse;

    UPROPERTY()
    FVector2D BasePosition = FVector2D::ZeroVector;

    UPROPERTY()
    float FloatPhase = 0.0f;

    UPROPERTY()
    float Scale = 1.0f;
};

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisInventoryCursePageWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Curse")
    void RefreshAll();

    UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Curse")
    void ClearSelectedCurse();

    UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Curse")
    bool HasSelectedCurse() const;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION()
    void HandleCurseEntryClicked(FMortisCurseInstance ClickedCurse);

    UFUNCTION()
    void HandleBackgroundCloseClicked();

    UFUNCTION()
    void HandleCurseAdded(const FMortisCurseInstance& AddedCurse);

    UFUNCTION()
    void HandleCurseRemoved(const FMortisCurseInstance& RemovedCurse);

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UCanvasPanel> Canvas_CurseField = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UWidget> Widget_HeadAnchor = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UMortisCurseDetailWidget> Widget_CurseDetail = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UButton> Button_BackgroundClose = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UButton> Button_CloseDetailArea = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UTextBlock> Text_EmptyMessage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Curse", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UMortisCurseEntryWidget> CurseEntryClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Style", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> FixedCurseIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (ClampMin = "1.0", ClampMax = "180.0", AllowPrivateAccess = "true"))
    float PrimaryArcDegrees = 140.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (ClampMin = "1", AllowPrivateAccess = "true"))
    int32 PrimaryArcCapacity = 7;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (ClampMin = "1", AllowPrivateAccess = "true"))
    int32 ShrinkStartCount = 8;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (ClampMin = "2", AllowPrivateAccess = "true"))
    int32 SecondArcStartCount = 11;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
    float PrimaryRadius = 260.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
    float SecondRadius = 340.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (ClampMin = "0.1", ClampMax = "1.0", AllowPrivateAccess = "true"))
    float EntryScaleWhenCrowded = 0.84f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (ClampMin = "0.1", ClampMax = "1.0", AllowPrivateAccess = "true"))
    float SecondArcEntryScale = 0.72f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (AllowPrivateAccess = "true"))
    FVector2D EntryWidgetSize = FVector2D(96.0f, 104.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
    float FloatAmplitude = 7.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
    float FloatSpeed = 1.4f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (AllowPrivateAccess = "true"))
    FVector2D DetailPopupOffset = FVector2D(32.0f, -24.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Inventory|Curse|Layout", meta = (AllowPrivateAccess = "true"))
    FVector2D DefaultDetailSize = FVector2D(320.0f, 180.0f);

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|Inventory|Curse", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMortisCurseInventorySubsystem> CurseInventorySubsystemRef = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|Inventory|Curse", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMortisCurseDatabaseSubsystem> CurseDatabaseSubsystemRef = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Curse", meta = (AllowPrivateAccess = "true"))
    FMortisCurseInstance SelectedCurse;

    UPROPERTY(Transient)
    TArray<FMortisCurseEntryRuntimeData> CreatedCurseEntries;

private:
    void InitializeReferences();
    void BindInventoryDelegates();
    void UnbindInventoryDelegates();
    void BindBackgroundButtons();
    void UnbindBackgroundButtons();

    void RefreshCurseEntries();
    void ClearCurseEntries();
    void RefreshEntrySelectionStates();
    void UpdateEmptyState();
    void UpdateDetailPanel();
    void HideDetailPanel();

    void LayoutCurseEntries();
    void PlaceArcEntries(int32 StartIndex, int32 EntryCount, const FVector2D& AnchorPosition, float Radius, float ArcDegrees, float EntryScale, int32 ZOrder);
    void UpdateFloatingEntries(float InDeltaTime);
    void PositionDetailNearSelectedEntry();

    FMortisCurseEntryViewData BuildEntryViewData(const FMortisCurseInstance& Curse) const;
    FMortisCurseDetailViewData BuildDetailViewData(const FMortisCurseInstance& Curse) const;
    FText BuildCurseDescriptionText(const FMortisCurseInstance& Curse, const FMortisCurseRow* CurseRow) const;
    UTexture2D* ResolveCurseIcon() const;

    bool IsSelectedCurse(const FMortisCurseInstance& Curse) const;
    bool DoesCurseExist(const FMortisCurseInstance& Curse) const;
    FMortisCurseEntryRuntimeData* FindRuntimeDataByCurseId(const FGuid& CurseId);
    const FMortisCurseRow* GetCurseRow(const FMortisCurseInstance& Curse) const;
    FVector2D ResolveHeadAnchorPosition() const;
    FVector2D ResolveCanvasSize() const;

private:
    float FloatingElapsedTime = 0.0f;
};
