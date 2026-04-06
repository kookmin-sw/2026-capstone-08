// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/MortisAnimInstanceBase.h"
#include "MortisPlayerLinkedAnimLayer.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisPlayerLinkedAnimLayer : public UMortisAnimInstanceBase
{
	GENERATED_BODY()
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UMortisPlayerAnimInstance* GetPlayerAnimInstance() const;
};
