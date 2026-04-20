// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/MortisAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/UI/MortisUIComponent.h"
#include "GameplayEffectExtension.h"
#include "MortisFunctionLibrary.h"
#include "MortisDebugHelper.h"
#include "MortisGameplayTags.h"

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
	// 체력 변경
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0, GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);

		if (UMortisUIComponent* UIComponent = GetUIComponentFromAttributeData(Data))
		{
			UIComponent->OnHealthChanged.Broadcast(NewCurrentHealth, GetMaxHealth());
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
		}

		// TODO: 사망 상태 태그 추가
		if (GetCurrentHealth() <= 0.0f)
		{
			Data.Target.AddLooseGameplayTag(MortisGameplayTags::State_Dead);
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(), MortisGameplayTags::Event_Dead, FGameplayEventData());
		}
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingPoiseDamageAttribute())
	{
		float NewPoise = FMath::Clamp(GetCurrentPoise() - GetIncomingPoiseDamage(), 0.f, GetMaxPoise());
		SetCurrentPoise(NewPoise);
		SetIncomingPoiseDamage(0.f);
		
		// if Poise == 0, send HitReact Event to target
		if (FMath::IsNearlyZero(NewPoise))
		{
			FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
			FGameplayEventData EventData;
			EventData.Instigator = Context.GetInstigator();
			EventData.Target = Data.Target.GetAvatarActor();
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				GetOwningActor(),
				MortisGameplayTags::Event_Action_HitReact,
				EventData
			);
			// Reset MaxPoise
			SetCurrentPoise(GetMaxPoise());
		}
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
