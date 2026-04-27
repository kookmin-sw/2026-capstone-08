// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "MortisCurseDataTypes.generated.h"

class UGameplayEffect;
class UTexture2D;

USTRUCT(BlueprintType)
struct FMortisCurseValueRange
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxValue = 0.f;
};

USTRUCT(BlueprintType)
struct FMortisCurseInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid InstanceId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag CurseTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RolledValue = 0.f;
};

USTRUCT(BlueprintType)
struct FMortisCurseRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag CurseTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DescriptionFormat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> CurseEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMortisCurseValueRange ValueRange;
};