// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/MortisAttributeSet.h"
#include "GameplayEffectExtension.h"

UMortisAttributeSet::UMortisAttributeSet()
{
	InitMaxHealth(1.0f);
	InitCurrentHealth(1.0f);
	InitBaseDamage(1.0f);
}

void UMortisAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// 체력 변경
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0, GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);

		// TODO: UI 컴포넌트의 Delegate에 Broadcast
	}
	// 데미지를 입었을 때
	else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float OldHealth = GetCurrentHealth();
		const float DamageTaken = GetIncomingDamage();

		const float NewHealth = FMath::Clamp(OldHealth - DamageTaken, 0, GetMaxHealth());
		SetCurrentHealth(NewHealth);

		// TODO: UI 컴포넌트의 Delegate에 Broadcast
		// TODO: 사망 상태 태그 추가
	}

}
