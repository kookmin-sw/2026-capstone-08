// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MortisAttributeSet.h"
#include "AbilitySystem/Attributes/MortisAttributeSet.h"
#include "MortisEnemyAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisEnemyAttributeSet : public UMortisAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Resist")
	FGameplayAttributeData ResistPierce;
	ATTRIBUTE_ACCESSORS(UMortisEnemyAttributeSet, ResistPierce)

	UPROPERTY(BlueprintReadOnly, Category = "Resist")
	FGameplayAttributeData ResistBlunt;
	ATTRIBUTE_ACCESSORS(UMortisEnemyAttributeSet, ResistBlunt)

	UPROPERTY(BlueprintReadOnly, Category = "Resist")
	FGameplayAttributeData ResistSlash;
	ATTRIBUTE_ACCESSORS(UMortisEnemyAttributeSet, ResistSlash)

	UPROPERTY(BlueprintReadOnly, Category = "Resist")
	FGameplayAttributeData ResistMagic;
	ATTRIBUTE_ACCESSORS(UMortisEnemyAttributeSet, ResistMagic)

	UMortisEnemyAttributeSet();
};
