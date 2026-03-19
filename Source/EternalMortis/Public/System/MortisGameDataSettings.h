// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MortisGameDataSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Mortis Game Data"))
class ETERNALMORTIS_API UMortisGameDataSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
    UPROPERTY(Config, EditAnywhere, Category = "Rune")
    TSoftObjectPtr<UDataTable> RuneSymbolTable;

    UPROPERTY(Config, EditAnywhere, Category = "Rune")
    TSoftObjectPtr<UDataTable> RuneSetTable;

    UPROPERTY(Config, EditAnywhere, Category = "Rune")
    TSoftObjectPtr<UDataTable> RuneDropRuleTable;
};
