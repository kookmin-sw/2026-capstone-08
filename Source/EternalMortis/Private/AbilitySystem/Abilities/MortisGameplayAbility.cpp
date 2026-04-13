// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MortisGameplayAbility.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "Character/MortisCharacterBase.h"
#include "Components/Combat/MortisCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "MortisDebugHelper.h"
#include "MortisFunctionLibrary.h"

void UMortisGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (AbilityActivationPolicy == EMortisAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
	}
}

void UMortisGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActorInfo && AbilityActivationPolicy == EMortisAbilityActivationPolicy::OnGiven)
		ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
}

FActiveGameplayEffectHandle UMortisGameplayAbility::NativeApplyEffectSpecHandleToTarget(const AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(const_cast<AActor*>(TargetActor));
	if (!TargetActor)
	{
		MORTIS_LOG("TargetActor is null");
	}
	if (!SpecHandle.IsValid())
	{
		MORTIS_LOG("SpecHandle is invalid");
	}
	// MORTIS_LOG("Apply GE: Target=%s", *TargetActor->GetActorNameOrLabel());
	return GetMortisAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*SpecHandle.Data,
		TargetASC
	);
}

FActiveGameplayEffectHandle UMortisGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle, EMortisSuccessType& OutSuccessType)
{
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, SpecHandle);
	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? EMortisSuccessType::Success : EMortisSuccessType::Failed;

	return ActiveGameplayEffectHandle;
}

void UMortisGameplayAbility::ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& SpecHandle, const TArray<FHitResult>& HitResults)
{
	if (HitResults.IsEmpty()) return;

	APawn* OwningPawn = CastChecked<APawn>(GetAvatarActorFromActorInfo());

	for (const FHitResult& Hit : HitResults)
	{
		if (APawn* HitPawn = Cast<APawn>(Hit.GetActor()))
		{
			if (UMortisFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(HitPawn, SpecHandle);

				if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					// 타격 이벤트 보내기
				}
			}
		}
	}
}

FGameplayEffectSpecHandle UMortisGameplayAbility::MakeEffectSpecWithSetByCaller(TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag DataTag, float Value, float EffectLevel) const
{
	if (!EffectClass)
		return FGameplayEffectSpecHandle();

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, EffectLevel);
	if (!SpecHandle.IsValid())
		return FGameplayEffectSpecHandle();

	SpecHandle.Data->SetSetByCallerMagnitude(DataTag, Value);
	return SpecHandle;
}

UMortisCombatComponent* UMortisGameplayAbility::GetMortisCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UMortisCombatComponent>();
}

UMortisAbilitySystemComponent* UMortisGameplayAbility::GetMortisAbilitySystemComponentFromActorInfo() const
{
	return Cast<UMortisAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

AMortisCharacterBase* UMortisGameplayAbility::GetMortisCharacterFromActorInfo() const
{
	return Cast<AMortisCharacterBase>(GetAvatarActorFromActorInfo());
}

UMotionWarpingComponent* UMortisGameplayAbility::GetMotionWarpingComponent() const
{
	if (const AMortisCharacterBase* Character = GetMortisCharacterFromActorInfo())
	{
		return Character->GetMotionWarpingComponent();
	}
	return nullptr;
}

FGameplayEffectSpecHandle UMortisGameplayAbility::MakeDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass,
	float Damage, float PoiseDamage, const FGameplayTag DamageTag)
{
	if (!EffectClass)
	{
		return FGameplayEffectSpecHandle();
	}

	FGameplayEffectContextHandle ContextHandle = GetMortisAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle EffectSpecHandle = GetMortisAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);

	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		DamageTag,
		Damage
	);
	
	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		MortisGameplayTags::Data_Stat_PoiseDamage,
		PoiseDamage
	);
	return EffectSpecHandle;
}
