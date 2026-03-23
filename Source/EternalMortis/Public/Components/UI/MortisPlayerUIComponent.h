// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/MortisUIComponent.h"
#include "MortisPlayerUIComponent.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, NewStamina, float, MaxStamina);

UCLASS(BlueprintType, Blueprintable, ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class ETERNALMORTIS_API UMortisPlayerUIComponent : public UMortisUIComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnStaminaChanged OnStaminaChanged;
};
