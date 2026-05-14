// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MortisShopPricingSubsystem.h"

#include "Engine/DataTable.h"
#include "System/MortisGameDataSettings.h"

void UMortisShopPricingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UMortisGameDataSettings* Settings = GetDefault<UMortisGameDataSettings>();
	check(Settings);

	ShopPriceRangeTable = Settings->ShopPriceRangeTable.LoadSynchronous();
	if (!ensureMsgf(ShopPriceRangeTable, TEXT("ShopPriceRangeTable is not set in MortisGameDataSettings.")))
	{
		return;
	}

	BuildPriceRangeCache();
}

bool UMortisShopPricingSubsystem::GenerateWeaponPrice(const FMortisWeaponRow& WeaponRow, int32& OutPrice) const
{
	return GeneratePrice(
		EMortisShopPriceItemType::Weapon,
		ToShopPriceGrade(WeaponRow.Grade),
		OutPrice);
}

bool UMortisShopPricingSubsystem::GenerateRunePrice(const FMortisRuneInstance& RuneInstance, int32& OutPrice) const
{
	return GeneratePrice(
		EMortisShopPriceItemType::Rune,
		ToShopPriceGrade(RuneInstance.Grade),
		OutPrice);
}

void UMortisShopPricingSubsystem::BuildPriceRangeCache()
{
	PriceRangesByKey.Empty();

	if (!ensureMsgf(ShopPriceRangeTable, TEXT("ShopPriceRangeTable is not Correct!")))
	{
		return;
	}

	TArray<FMortisShopPriceRangeRow*> AllRows;
	ShopPriceRangeTable->GetAllRows(TEXT("BuildShopPriceRangeCache"), AllRows);

	for (const FMortisShopPriceRangeRow* Row : AllRows)
	{
		if (!Row)
		{
			continue;
		}

		if (!ensureMsgf(Row->IsValidRange(),
			TEXT("Invalid shop price range row. ItemType=%s Grade=%s MinPrice=%d MaxPrice=%d"),
			*GetEnumValueName(Row->ItemType),
			*GetEnumValueName(Row->Grade),
			Row->MinPrice,
			Row->MaxPrice))
		{
			continue;
		}

		const FName Key = MakePriceRangeKey(Row->ItemType, Row->Grade);
		if (!ensureMsgf(!PriceRangesByKey.Contains(Key), TEXT("Duplicate shop price range row for key %s."), *Key.ToString()))
		{
			continue;
		}

		PriceRangesByKey.Add(Key, *Row);
	}

	const EMortisShopPriceItemType RequiredItemTypes[] =
	{
		EMortisShopPriceItemType::Weapon,
		EMortisShopPriceItemType::Rune
	};

	const EMortisShopPriceGrade RequiredGrades[] =
	{
		EMortisShopPriceGrade::Common,
		EMortisShopPriceGrade::Rare,
		EMortisShopPriceGrade::Epic,
		EMortisShopPriceGrade::Legendary
	};

	for (const EMortisShopPriceItemType ItemType : RequiredItemTypes)
	{
		for (const EMortisShopPriceGrade Grade : RequiredGrades)
		{
			const FName Key = MakePriceRangeKey(ItemType, Grade);
			ensureMsgf(PriceRangesByKey.Contains(Key), TEXT("Missing shop price range row for key %s."), *Key.ToString());
		}
	}
}

bool UMortisShopPricingSubsystem::GeneratePrice(EMortisShopPriceItemType ItemType, EMortisShopPriceGrade Grade, int32& OutPrice) const
{
	OutPrice = 0;

	const FMortisShopPriceRangeRow* PriceRange = FindPriceRange(ItemType, Grade);
	if (!PriceRange)
	{
		ensureMsgf(false,
			TEXT("Missing shop price range row. ItemType=%s Grade=%s"),
			*GetEnumValueName(ItemType),
			*GetEnumValueName(Grade));
		return false;
	}

	if (!PriceRange->IsValidRange())
	{
		ensureMsgf(false,
			TEXT("Invalid shop price range. ItemType=%s Grade=%s MinPrice=%d MaxPrice=%d"),
			*GetEnumValueName(ItemType),
			*GetEnumValueName(Grade),
			PriceRange->MinPrice,
			PriceRange->MaxPrice);
		return false;
	}

	OutPrice = FMath::RandRange(PriceRange->MinPrice, PriceRange->MaxPrice);
	return true;
}

const FMortisShopPriceRangeRow* UMortisShopPricingSubsystem::FindPriceRange(EMortisShopPriceItemType ItemType, EMortisShopPriceGrade Grade) const
{
	return PriceRangesByKey.Find(MakePriceRangeKey(ItemType, Grade));
}

EMortisShopPriceGrade UMortisShopPricingSubsystem::ToShopPriceGrade(EMortisWeaponGrade WeaponGrade)
{
	switch (WeaponGrade)
	{
	case EMortisWeaponGrade::Rare:
		return EMortisShopPriceGrade::Rare;
	case EMortisWeaponGrade::Epic:
		return EMortisShopPriceGrade::Epic;
	case EMortisWeaponGrade::Legendary:
		return EMortisShopPriceGrade::Legendary;
	case EMortisWeaponGrade::Common:
	default:
		return EMortisShopPriceGrade::Common;
	}
}

EMortisShopPriceGrade UMortisShopPricingSubsystem::ToShopPriceGrade(EMortisRuneGrade RuneGrade)
{
	switch (RuneGrade)
	{
	case EMortisRuneGrade::Rare:
		return EMortisShopPriceGrade::Rare;
	case EMortisRuneGrade::Epic:
		return EMortisShopPriceGrade::Epic;
	case EMortisRuneGrade::Legendary:
		return EMortisShopPriceGrade::Legendary;
	case EMortisRuneGrade::Common:
	default:
		return EMortisShopPriceGrade::Common;
	}
}

FName UMortisShopPricingSubsystem::MakePriceRangeKey(EMortisShopPriceItemType ItemType, EMortisShopPriceGrade Grade)
{
	return FName(*FString::Printf(TEXT("%s_%s"), *GetEnumValueName(ItemType), *GetEnumValueName(Grade)));
}

FString UMortisShopPricingSubsystem::GetEnumValueName(EMortisShopPriceItemType ItemType)
{
	if (const UEnum* Enum = StaticEnum<EMortisShopPriceItemType>())
	{
		return Enum->GetNameStringByValue(static_cast<int64>(ItemType));
	}

	return TEXT("UnknownItemType");
}

FString UMortisShopPricingSubsystem::GetEnumValueName(EMortisShopPriceGrade Grade)
{
	if (const UEnum* Enum = StaticEnum<EMortisShopPriceGrade>())
	{
		return Enum->GetNameStringByValue(static_cast<int64>(Grade));
	}

	return TEXT("UnknownGrade");
}
