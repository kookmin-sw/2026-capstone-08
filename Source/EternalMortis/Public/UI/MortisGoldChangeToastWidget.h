#pragma once

#include "CoreMinimal.h"
#include "UI/MortisWidgetBase.h"
#include "MortisGoldChangeToastWidget.generated.h"

class UBorder;
class UHorizontalBox;
class UHorizontalBoxSlot;
class UImage;
class UMortisMetaProgressionSubsystem;
class UMortisRunStateSubsystem;
class USizeBox;
class UTextBlock;
class UTexture2D;

enum class EMortisCurrencyToastKind : uint8
{
    MemoryFragments,
    Gold
};

struct FMortisCurrencyToastEntry
{
    USizeBox* RootBox = nullptr;
    UBorder* Border = nullptr;
    UImage* IconImage = nullptr;
    UTextBlock* TotalText = nullptr;
    UTextBlock* DeltaText = nullptr;
    UHorizontalBoxSlot* Slot = nullptr;

    int32 DisplayedTotal = 0;
    int32 StartTotal = 0;
    int32 TargetTotal = 0;
    int32 PendingDelta = 0;
    float AnimationElapsed = 0.0f;
    bool bIsAnimating = false;
    bool bIsActive = false;
};

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisGoldChangeToastWidget : public UMortisWidgetBase
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    UFUNCTION(BlueprintCallable, Category = "Mortis|HUD|Currency")
    void InitializeFromRunState();

    UFUNCTION(BlueprintCallable, Category = "Mortis|HUD|Currency")
    void RefreshVisual();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION()
    void HandleGoldChanged(int32 DeltaGold);

    UFUNCTION()
    void HandleMemoryFragmentsChanged(int32 NewValue);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Style", meta = (AllowPrivateAccess = "true"))
    FVector2D EntrySize = FVector2D(180.0f, 56.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Style", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
    float EntrySpacing = 12.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Style", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> GoldIcon = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Style", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTexture2D> MemoryFragmentIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Style", meta = (AllowPrivateAccess = "true"))
    FLinearColor GainDeltaColor = FLinearColor(1.0f, 0.82f, 0.18f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Style", meta = (AllowPrivateAccess = "true"))
    FLinearColor SpendDeltaColor = FLinearColor(0.95f, 0.24f, 0.16f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Style", meta = (AllowPrivateAccess = "true"))
    FLinearColor TotalTextColor = FLinearColor(0.98f, 0.91f, 0.68f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Style", meta = (AllowPrivateAccess = "true"))
    FVector2D DeltaVisibleTranslation = FVector2D(0.0f, 24.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Style", meta = (AllowPrivateAccess = "true"))
    FVector2D DeltaAbsorbedTranslation = FVector2D::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Timing", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
    float CoalesceDuration = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Timing", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
    float DeltaAppearDuration = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Timing", meta = (ClampMin = "0.01", AllowPrivateAccess = "true"))
    float AbsorbDuration = 0.65f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|HUD|Currency|Timing", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
    float HoldDuration = 0.40f;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|HUD|Currency", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMortisRunStateSubsystem> RunStateSubsystemRef = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Mortis|HUD|Currency", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMortisMetaProgressionSubsystem> MetaProgressionSubsystemRef = nullptr;

private:
    void BuildFallbackWidgetTreeIfNeeded();
    void BuildCurrencyEntry(FMortisCurrencyToastEntry& Entry, EMortisCurrencyToastKind Kind);
    void BindSubsystemDelegates();
    void UnbindSubsystemDelegates();
    void QueueCurrencyChange(EMortisCurrencyToastKind Kind, int32 Delta, int32 TargetTotal);
    void FlushPendingCurrencyChanges();
    void StartEntryAnimation(FMortisCurrencyToastEntry& Entry, int32 Delta, int32 TargetTotal);
    void UpdateEntryAnimation(FMortisCurrencyToastEntry& Entry, float InDeltaTime);
    void FinishEntryAnimation(FMortisCurrencyToastEntry& Entry);
    void RefreshEntryVisual(FMortisCurrencyToastEntry& Entry, UTexture2D* Icon) const;
    void UpdateEntryLayout();
    void ShowPanel();
    void HidePanelIfIdle();
    bool HasActiveEntries() const;
    bool IsEntryVisible(const FMortisCurrencyToastEntry& Entry) const;
    FMortisCurrencyToastEntry& GetEntry(EMortisCurrencyToastKind Kind);

private:
    UPROPERTY(Transient)
    TObjectPtr<UHorizontalBox> HorizontalBox_Root = nullptr;

    FMortisCurrencyToastEntry MemoryFragmentsEntry;
    FMortisCurrencyToastEntry GoldEntry;

    int32 LastKnownMemoryFragments = 0;

    int32 PendingGoldDelta = 0;
    int32 PendingGoldTarget = 0;
    bool bHasPendingGold = false;

    int32 PendingMemoryDelta = 0;
    int32 PendingMemoryTarget = 0;
    bool bHasPendingMemoryFragments = false;

    float CoalesceElapsed = 0.0f;
    bool bIsCoalescing = false;
};
