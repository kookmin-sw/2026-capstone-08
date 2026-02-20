// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/MortisStructTypes.h"
#include "AbilitySystem/Abilities/MortisPlayerGameplayAbility.h"

bool FMortisAbilityInputBinding::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}