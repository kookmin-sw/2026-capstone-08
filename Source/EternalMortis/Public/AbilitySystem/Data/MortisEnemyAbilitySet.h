// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/MortisAbilitySetBase.h"
#include "MortisEnemyAbilitySet.generated.h"

struct FMortisEnemyStats;
class UMortisEnemyGameplayAbility;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisEnemyAbilitySet : public UMortisAbilitySetBase
{
	GENERATED_BODY()

public:
	void GiveToEnemy(UMortisAbilitySystemComponent* ASC, const FMortisEnemyStats& EnemyStats, int32 ApplyLevel = 1);
	virtual void GiveToAbilitySystemComponent(UMortisAbilitySystemComponent* ASCToGive, int32 ApplyLevel = 1) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "InitData")
	TArray<TSubclassOf<UMortisEnemyGameplayAbility>> EnemyAbilities;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	TSubclassOf<UGameplayEffect> StatsEffectClass;
};
