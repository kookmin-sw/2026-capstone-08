#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/MortisMetaProgressionDataTypes.h"
#include "UI/MortisWidgetBase.h"
#include "MortisUpgradeIconEntryWidget.generated.h"

class UBorder;
class UButton;
class UImage;
class UTextBlock;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMortisOnUpgradeEntryClicked, FGameplayTag, ExperienceTag, bool, bNoUpgradeEntry);

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisUpgradeIconEntryWidget : public UMortisWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression|Entry")
	void InitializeEntry(const FMortisExperienceRow& InExperienceRow, bool bInUnlocked, bool bInActive);

	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression|Entry")
	void InitializeManualEntry(FGameplayTag InExperienceTag, const FText& InDisplayName, UTexture2D* InIcon, bool bInUnlocked, bool bInActive, bool bInNoUpgradeEntry);

	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression|Entry")
	void SetActive(bool bInActive);

	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression|Entry")
	void SetUnlocked(bool bInUnlocked);

	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression|Entry")
	void RefreshVisual();

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression|Entry")
	FGameplayTag GetExperienceTag() const { return ExperienceTag; }

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression|Entry")
	bool IsUnlocked() const { return bUnlocked; }

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression|Entry")
	bool IsActive() const { return bActive; }

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression|Entry")
	bool IsNoUpgradeEntry() const { return bNoUpgradeEntry; }

	UPROPERTY(BlueprintAssignable, Category = "Mortis|MetaProgression|Entry")
	FMortisOnUpgradeEntryClicked OnUpgradeEntryClicked;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleIconClicked();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UButton> BTN_Icon = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UImage> IMG_Icon = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UBorder> BD_LockDim = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UImage> IMG_Lock = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UImage> IMG_ActiveRing = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TXT_Name = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|MetaProgression|Entry", meta = (AllowPrivateAccess = "true"))
	FGameplayTag ExperienceTag;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|MetaProgression|Entry", meta = (AllowPrivateAccess = "true"))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|MetaProgression|Entry", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|MetaProgression|Entry", meta = (AllowPrivateAccess = "true"))
	bool bUnlocked = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|MetaProgression|Entry", meta = (AllowPrivateAccess = "true"))
	bool bActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|MetaProgression|Entry", meta = (AllowPrivateAccess = "true"))
	bool bNoUpgradeEntry = false;
};
