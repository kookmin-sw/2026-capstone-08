#pragma once

#include "CoreMinimal.h"
#include "UI/MortisWidgetBase.h"
#include "MortisRuneSlotIconWidget.generated.h"

class UImage;
class UTexture2D;

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisRuneSlotIconWidget : public UMortisWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression|RuneSlot")
	void InitializeSlot(int32 InSlotIndex, bool bInUnlocked);

	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression|RuneSlot")
	void SetUnlocked(bool bInUnlocked);

	UFUNCTION(BlueprintCallable, Category = "Mortis|MetaProgression|RuneSlot")
	void RefreshVisual();

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression|RuneSlot")
	int32 GetSlotIndex() const { return SlotIndex; }

	UFUNCTION(BlueprintPure, Category = "Mortis|MetaProgression|RuneSlot")
	bool IsUnlocked() const { return bUnlocked; }

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UImage> IMG_RuneSlotIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTexture2D> UnlockedIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTexture2D> LockedIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
	float UnlockedOpacity = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
	float LockedOpacity = 0.35f;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot", meta = (AllowPrivateAccess = "true"))
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|MetaProgression|RuneSlot", meta = (AllowPrivateAccess = "true"))
	bool bUnlocked = false;
};
