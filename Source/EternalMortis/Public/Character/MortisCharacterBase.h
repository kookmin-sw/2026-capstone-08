// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

#include "Interfaces/MortisCombatInterface.h"
#include "MortisCharacterBase.generated.h"

class UMortisAbilitySystemComponent;
class UMortisAttributeSet;
class UMortisAbilitySetBase;

UCLASS()
class ETERNALMORTIS_API AMortisCharacterBase : public ACharacter, public IAbilitySystemInterface, public IMortisCombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMortisCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// IAbilitySystemInterface Override
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// IMortisCombatInterface Override
	FORCEINLINE virtual UMortisCombatComponent* GetCombatComponent() const override { return nullptr; }
	
protected:
	// APawn Override
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UMortisAbilitySystemComponent* MortisAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UMortisAttributeSet* MortisAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem")
	TSoftObjectPtr<UMortisAbilitySetBase> CharacterAbilitySet;


public:
	FORCEINLINE UMortisAbilitySystemComponent* GetMortisAbilitySystemComponent() const { return MortisAbilitySystemComponent; }
	FORCEINLINE UMortisAttributeSet* GetMortisAttributeSet() const { return MortisAttributeSet; }
};
