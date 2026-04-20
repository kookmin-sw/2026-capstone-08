// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UI/MortisEnemyUIComponent.h"

#include "AbilitySystem/Attributes/MortisAttributeSet.h"
#include "Character/MortisCharacterBase.h"

bool UMortisEnemyUIComponent::GetCurrentHealthSnapshot(float& OutCurrentHealth, float& OutMaxHealth) const
{
	OutCurrentHealth = 0.f;
	OutMaxHealth = 0.f;

	const AMortisCharacterBase* OwnerCharacter = Cast<AMortisCharacterBase>(GetOwner());
	const UMortisAttributeSet* AttributeSet = OwnerCharacter ? OwnerCharacter->GetMortisAttributeSet() : nullptr;
	if (!AttributeSet)
	{
		return false;
	}

	OutMaxHealth = FMath::Max(AttributeSet->GetMaxHealth(), 0.f);
	OutCurrentHealth = OutMaxHealth > 0.f
		? FMath::Clamp(AttributeSet->GetCurrentHealth(), 0.f, OutMaxHealth)
		: 0.f;
	return true;
}
