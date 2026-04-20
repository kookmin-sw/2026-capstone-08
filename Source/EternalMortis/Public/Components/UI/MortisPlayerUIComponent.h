// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/MortisUIComponent.h"
#include "MortisPlayerUIComponent.generated.h"

class UMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, NewStamina, float, MaxStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnManaChanged, float, NewMana, float, MaxMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPotionCountChanged, int32, NewPotionCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponMeshChanged, UMeshComponent*, NewWeaponMesh);

UCLASS(BlueprintType, Blueprintable, ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class ETERNALMORTIS_API UMortisPlayerUIComponent : public UMortisUIComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnStaminaChanged OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnManaChanged OnManaChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnPotionCountChanged OnPotionCountChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnEquippedWeaponMeshChanged OnEquippedWeaponMeshChanged;
	
};
