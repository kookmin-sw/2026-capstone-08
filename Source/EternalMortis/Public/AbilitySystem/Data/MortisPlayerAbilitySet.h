// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/MortisAbilitySetBase.h"
#include "Types/MortisStructTypes.h"
#include "MortisPlayerAbilitySet.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisPlayerAbilitySet : public UMortisAbilitySetBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "InitData", meta = (TitleProperty = "InputTag"))
	TArray<FMortisAbilityInputBinding> PlayerInputAbilities;
	
public:
	virtual void GiveToAbilitySystemComponent(UMortisAbilitySystemComponent* ASCToGive, int32 ApplyLevel = 1) override;
	
};
