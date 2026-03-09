// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameplayTagContainer.h"
#include "MortisBoxComponent.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBoxComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Hitbox")
	FGameplayTag CollisionTag;
};
