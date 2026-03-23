// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/MortisPlayerAttributeSet.h"
#include "Character/Player/MortisPlayerCharacter.h"
#include "Components/UI/MortisPlayerUIComponent.h"
#include "GameplayEffectExtension.h"

namespace
{
	UMortisPlayerUIComponent* GetPlayerUIComponentFromPlayerAttributeData(const FGameplayEffectModCallbackData& Data)
	{
		const AActor* OwningActor = Data.Target.GetAvatarActor();
		const AMortisPlayerCharacter* PlayerCharacter = Cast<AMortisPlayerCharacter>(OwningActor);

		return PlayerCharacter ? PlayerCharacter->FindComponentByClass<UMortisPlayerUIComponent>() : nullptr;
	}
}

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

		if (UMortisPlayerUIComponent* PlayerUIComponent = GetPlayerUIComponentFromPlayerAttributeData(Data))
		{
			PlayerUIComponent->OnStaminaChanged.Broadcast(NewCurrentStamina, GetMaxStamina());
		}
	}
}
