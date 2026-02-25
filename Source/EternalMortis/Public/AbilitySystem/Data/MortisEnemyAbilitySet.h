// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/MortisAbilitySetBase.h"
#include "MortisEnemyAbilitySet.generated.h"

class UMortisEnemyGameplayAbility;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisEnemyAbilitySet : public UMortisAbilitySetBase
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UMortisAbilitySystemComponent* ASCToGive, int32 ApplyLevel = 1) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "InitData")
	TArray<TSubclassOf<UMortisEnemyGameplayAbility>> EnemyAbilities;
};
