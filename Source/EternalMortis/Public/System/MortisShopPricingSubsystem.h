// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/MortisRuneDataTypes.h"
#include "Types/MortisShopPriceTypes.h"
#include "Types/MortisWeaponDataTypes.h"
#include "MortisShopPricingSubsystem.generated.h"

class UDataTable;

UCLASS()
class ETERNALMORTIS_API UMortisShopPricingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Shop|Pricing")
	bool GenerateWeaponPrice(const FMortisWeaponRow& WeaponRow, int32& OutPrice) const;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Shop|Pricing")
	bool GenerateRunePrice(const FMortisRuneInstance& RuneInstance, int32& OutPrice) const;

protected:
	void BuildPriceRangeCache();

	bool GeneratePrice(EMortisShopPriceItemType ItemType, EMortisShopPriceGrade Grade, int32& OutPrice) const;
	const FMortisShopPriceRangeRow* FindPriceRange(EMortisShopPriceItemType ItemType, EMortisShopPriceGrade Grade) const;

	static EMortisShopPriceGrade ToShopPriceGrade(EMortisWeaponGrade WeaponGrade);
	static EMortisShopPriceGrade ToShopPriceGrade(EMortisRuneGrade RuneGrade);
	static FName MakePriceRangeKey(EMortisShopPriceItemType ItemType, EMortisShopPriceGrade Grade);
	static FString GetEnumValueName(EMortisShopPriceItemType ItemType);
	static FString GetEnumValueName(EMortisShopPriceGrade Grade);

private:
	UPROPERTY()
	TObjectPtr<UDataTable> ShopPriceRangeTable = nullptr;

	TMap<FName, FMortisShopPriceRangeRow> PriceRangesByKey;
};
