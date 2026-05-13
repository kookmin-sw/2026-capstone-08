// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MortisShopPriceTypes.generated.h"

UENUM(BlueprintType)
enum class EMortisShopPriceItemType : uint8
{
	Weapon	UMETA(DisplayName = "Weapon"),
	Rune	UMETA(DisplayName = "Rune")
};

UENUM(BlueprintType)
enum class EMortisShopPriceGrade : uint8
{
	Common		UMETA(DisplayName = "Common"),
	Rare		UMETA(DisplayName = "Rare"),
	Epic		UMETA(DisplayName = "Epic"),
	Legendary	UMETA(DisplayName = "Legendary")
};

USTRUCT(BlueprintType)
struct FMortisShopPriceRangeRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMortisShopPriceItemType ItemType = EMortisShopPriceItemType::Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMortisShopPriceGrade Grade = EMortisShopPriceGrade::Common;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinPrice = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxPrice = 0;

	bool IsValidRange() const
	{
		return MinPrice >= 0 && MaxPrice >= 0 && MaxPrice >= MinPrice;
	}
};
