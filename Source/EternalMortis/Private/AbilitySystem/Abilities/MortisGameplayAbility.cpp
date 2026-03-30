// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MortisGameplayAbility.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "Character/MortisCharacterBase.h"
#include "Components/Combat/MortisCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
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

FActiveGameplayEffectHandle UMortisGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	check(TargetASC && SpecHandle.IsValid());
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
