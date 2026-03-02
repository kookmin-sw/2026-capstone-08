// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MortisAbilitySetBase.generated.h"

class UMortisGameplayAbility;
class UMortisAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisAbilitySetBase : public UDataAsset
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "InitData")
	TArray<TSubclassOf<UMortisGameplayAbility>> ActivateOnGivenAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "InitData")
	TArray<TSubclassOf<UMortisGameplayAbility>> ReactiveAbilities;

	void GrantAbilities(const TArray<TSubclassOf<UMortisGameplayAbility>>& AbilitySet, UMortisAbilitySystemComponent* ASCToGive, int32 ApplyLevel = 1);

public:
	virtual void GiveToAbilitySystemComponent(UMortisAbilitySystemComponent* ASCToGive, int32 ApplyLevel = 1);
};
