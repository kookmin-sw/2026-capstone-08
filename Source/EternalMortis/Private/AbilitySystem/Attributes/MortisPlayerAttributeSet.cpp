// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/MortisPlayerAttributeSet.h"
#include "GameplayEffectExtension.h"

UMortisPlayerAttributeSet::UMortisPlayerAttributeSet()
{
	InitMaxStamina(1.0f);
	InitCurrentStamina(1.0f);
	InitStrength(10.0f);
	InitDexterity(10.0f);
	InitIntelligence(10.0f);
	InitWeaponDamage(0.0f);
	InitStrCoef(0.0f);
	InitDexCoef(0.0f);
	InitIntCoef(0.0f);
}

void UMortisPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentStaminaAttribute())
	{
		const float NewCurrentStamina = FMath::Clamp(GetCurrentStamina(), 0, GetMaxStamina());
		SetCurrentStamina(NewCurrentStamina);

		// TODO:: Player UI 컴포넌트의 Delegate에 Broadcast
	}
}
