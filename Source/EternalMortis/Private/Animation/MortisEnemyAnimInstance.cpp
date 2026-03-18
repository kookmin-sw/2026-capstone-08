// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MortisEnemyAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "Character/MortisCharacterBase.h"

void UMortisEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwningCharacter)
	{
		if (UAbilitySystemComponent* ASC = OwningCharacter->GetAbilitySystemComponent())
		{
			BindTagMappings(ASC);
		}
	}
}

void UMortisEnemyAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	
	if (UAnimMontage* CurrentMontage = GetCurrentActiveMontage())
	{
		float CurveSpeed = GetCurveValue(FName("PlayRateCurve"));
		if (CurveSpeed > 0.0f)
		{
			Montage_SetPlayRate(CurrentMontage, CurveSpeed);
		}
	}
}

void UMortisEnemyAnimInstance::BindTagMappings(UAbilitySystemComponent* ASC)
{
	for (const FMortisTagToBoolMapping& Mapping : TagToBoolMappings)
	{
		if (!Mapping.Tag.IsValid() || Mapping.BoolPropertyName.IsNone())
		{
			continue;
		}
		
		FBoolProperty* Prop = FindFProperty<FBoolProperty>(GetClass(), Mapping.BoolPropertyName);
		if (!Prop)
		{
			continue;
		}
		
		FOnGameplayEffectTagCountChanged::FDelegate Delegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(
			this,
			&ThisClass::OnTagChanged,
			Mapping.BoolPropertyName
		);
		
		ASC->RegisterAndCallGameplayTagEvent(
			Mapping.Tag,
			Delegate,	
			EGameplayTagEventType::NewOrRemoved
		);
	}
}

void UMortisEnemyAnimInstance::OnTagChanged(const FGameplayTag Tag, int32 NewCount, FName BoolPropertyName)
{
	if (FBoolProperty* Prop = FindFProperty<FBoolProperty>(GetClass(), BoolPropertyName))
	{
		Prop->SetPropertyValue_InContainer(this, NewCount > 0);
	}
}
