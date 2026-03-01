// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTags.h"
#include "MortisAnimInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	bool DoesOwnerHaveTag(const FGameplayTag& GameplayTag) const;
};
