#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Types/MortisRuneDataTypes.h"
#include "Types/MortisStructTypes.h"
#include "Types/MortisWeaponDataTypes.h"
#include "UI/MortisWidgetBase.h"
#include "MortisInventoryStatusPageWidget.generated.h"

class AMortisPlayerWeapon;
class AActor;
class UMeshComponent;

USTRUCT(BlueprintType)
struct FMortisInventoryStatusAttributeValue
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	FGameplayAttribute Attribute;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	FName AttributeName;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	TSubclassOf<UAttributeSet> AttributeSetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	FName AttributeSetName;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	FName CategoryName;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	float Value = 0.0f;
};

USTRUCT(BlueprintType)
struct FMortisInventoryStatusWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	bool bHasWeaponRow = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	FMortisWeaponRow WeaponRow;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	bool bHasEquippedWeaponActor = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	TObjectPtr<AMortisPlayerWeapon> EquippedWeaponActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	bool bHasEquippedWeaponMesh = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	TObjectPtr<UMeshComponent> EquippedWeaponMesh = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	FMortisPlayerWeaponData PlayerWeaponData;
};

USTRUCT(BlueprintType)
struct FMortisInventoryStatusRuneSetInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	FGameplayTag SetTag;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	int32 EquippedCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	bool bHasRuneSetRow = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	FMortisRuneSetRow RuneSetRow;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	bool bHasRuntimeState = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	FMortisActiveRuneSetState ActiveRuneSetState;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	bool bIsActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	int32 CurrentLevel = 0;
};

USTRUCT(BlueprintType)
struct FMortisInventoryStatusRuneStatSummary
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	EMortisRuneSymbol SymbolType = EMortisRuneSymbol::None;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	bool bHasRuneSymbolRow = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	FMortisRuneSymbolRow RuneSymbolRow;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	int32 RuneCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	float TotalValue = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	TArray<FMortisRuneInstance> SourceRunes;
};

USTRUCT(BlueprintType)
struct FMortisInventoryStatusSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	bool bHasPlayerActor = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	TObjectPtr<AActor> PlayerActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	bool bHasAbilitySystem = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	TArray<FMortisInventoryStatusAttributeValue> PlayerAttributes;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	FMortisInventoryStatusWeaponInfo WeaponInfo;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	TArray<FMortisRuneInstance> EquippedRunes;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	TArray<FGameplayTag> ActiveRuneSets;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	TArray<FMortisInventoryStatusRuneSetInfo> EquippedRuneSetInfos;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status")
	TArray<FMortisInventoryStatusRuneStatSummary> EquippedRuneStatSummaries;
};

UCLASS(BlueprintType, Blueprintable)
class ETERNALMORTIS_API UMortisInventoryStatusPageWidget : public UMortisWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Mortis|Inventory|Status")
	void RefreshStatusData(AActor* PlayerActor = nullptr);

	UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Status")
	FMortisInventoryStatusSnapshot GetStatusSnapshot() const { return CurrentStatusSnapshot; }

	UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Status")
	bool FindAttributeValueByName(FName AttributeName, FMortisInventoryStatusAttributeValue& OutValue) const;

	UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Status")
	bool FindRuneStatSummaryBySymbol(EMortisRuneSymbol SymbolType, FMortisInventoryStatusRuneStatSummary& OutSummary) const;

	UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Status")
	bool FindRuneBonusValueBySymbol(EMortisRuneSymbol SymbolType, float& OutBonusValue) const;

	UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Status")
	bool BuildAttributeTextByRuneSymbol(EMortisRuneSymbol SymbolType, const FText LeftText, FText& OutLeftText, FText& OutRightText) const;

	UFUNCTION(BlueprintPure, Category = "Mortis|Inventory|Status")
	bool BuildResourceTextByRuneSymbol(EMortisRuneSymbol SymbolType, const FText LeftText, FText& OutLeftText, FText& OutRightText) const;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Inventory|Status", meta = (AllowPrivateAccess = "true"))
	FMortisInventoryStatusSnapshot CurrentStatusSnapshot;

private:
	AActor* ResolvePlayerActor(AActor* ExplicitPlayerActor) const;
	void CollectPlayerAttributes(AActor* PlayerActor, FMortisInventoryStatusSnapshot& OutSnapshot) const;
	void CollectWeaponInfo(AActor* PlayerActor, FMortisInventoryStatusSnapshot& OutSnapshot) const;
	void CollectRuneInfo(FMortisInventoryStatusSnapshot& OutSnapshot) const;

	static bool GetAttributeNameByRuneSymbol(EMortisRuneSymbol SymbolType, FName& OutAttributeName);
	static bool GetResourceAttributeNamesByRuneSymbol(EMortisRuneSymbol SymbolType, FName& OutCurrentAttributeName, FName& OutMaxAttributeName);
	static FText FormatStatusNumber(float Value);
	static FText FormatRuneBonus(float RuneBonusValue);
	static FText FormatStatusValueWithRuneBonus(float Value, float RuneBonusValue);
};
