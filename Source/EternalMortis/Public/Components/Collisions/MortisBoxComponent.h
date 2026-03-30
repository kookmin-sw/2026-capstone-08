// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameplayTagContainer.h"
#include "MortisBoxComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class ETERNALMORTIS_API UMortisBoxComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	FGameplayTag GetCollisionTag() const { return CollisionTag; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Hitbox", meta = (Categories = "Data.CollisionType"))
	FGameplayTag CollisionTag;
};
