// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "MortisMetaProgressionDataTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FMortisExperienceRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ExperienceTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 UnlockCost = 1;

	// GE가 아닌 런 상태 보너스
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 StartGoldBonus = 0;

	// 경험 선택 시 적용할 GE 목록
	// 수치는 GE 내부에서 직접 관리한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> GrantedEffects;
};
