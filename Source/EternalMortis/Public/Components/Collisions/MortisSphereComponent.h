// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SphereComponent.h"
#include "MortisSphereComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class ETERNALMORTIS_API UMortisSphereComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	FGameplayTag GetCollisionTag() const { return CollisionTag; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Hitbox", meta = (Categories = "Data.CollisionType"))
	FGameplayTag CollisionTag;
};
