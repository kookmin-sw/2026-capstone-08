// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/MortisAttributeSet.h"
#include "Components/UI/MortisUIComponent.h"
#include "GameplayEffectExtension.h"
#include "MortisDebugHelper.h"

namespace
{
	UMortisUIComponent* GetUIComponentFromAttributeData(const FGameplayEffectModCallbackData& Data)
	{
		const AActor* OwningActor = Data.Target.GetAvatarActor();
		return OwningActor ? OwningActor->FindComponentByClass<UMortisUIComponent>() : nullptr;
	}
}

UMortisAttributeSet::UMortisAttributeSet()
{
	InitMaxHealth(1.0f);
	InitCurrentHealth(1.0f);
	InitBaseDamage(1.0f);
}

void UMortisAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	MORTIS_LOG("");
	// 체력 변경
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
			MORTIS_LOG("");
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0, GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);

		if (UMortisUIComponent* UIComponent = GetUIComponentFromAttributeData(Data))
		{
			UIComponent->OnHealthChanged.Broadcast(NewCurrentHealth, GetMaxHealth());
			MORTIS_LOG("");
		}
	}
	// 데미지를 입었을 때
	else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float OldHealth = GetCurrentHealth();
		const float DamageTaken = GetIncomingDamage();

		const float NewHealth = FMath::Clamp(OldHealth - DamageTaken, 0, GetMaxHealth());
		SetCurrentHealth(NewHealth);
		SetIncomingDamage(0.f);
		
		// TODO: UI 컴포넌트의 Delegate에 Broadcast

		if (UMortisUIComponent* UIComponent = GetUIComponentFromAttributeData(Data))
		{
			UIComponent->OnHealthChanged.Broadcast(NewHealth, GetMaxHealth());
			MORTIS_LOG("");
			
		}

		// TODO: 사망 상태 태그 추가
	}

}

void UMortisAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		if (NewValue > OldValue)
		{
			float CurrHealth = GetCurrentHealth();
			CurrHealth += (NewValue - OldValue);
			SetCurrentHealth(CurrHealth);
		}
		else if (GetCurrentHealth() > NewValue)
			SetCurrentHealth(NewValue);
	}
}
