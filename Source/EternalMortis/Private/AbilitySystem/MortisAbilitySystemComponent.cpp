// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/MortisPlayerGameplayAbility.h"
#include "MortisGameplayTags.h"
#include "MortisDebugHelper.h"

// Player 용
void UMortisAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
			TryActivateAbility(AbilitySpec.Handle);
		// break를 넣을지 말지 고민해볼 것
	}
}

void UMortisAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid() || !InputTag.MatchesTag(MortisGameplayTags::InputTag_Hold)) return;

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && AbilitySpec.IsActive())
			CancelAbilityHandle(AbilitySpec.Handle);
		// break를 넣을지 말지 고민해볼 것
	}
}

void UMortisAbilitySystemComponent::GrantPlayerWeaponAbilities(const TArray<FMortisAbilityInputBinding>& DefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& GrantedAbilitySpecHandles)
{
	if (DefaultWeaponAbilities.IsEmpty()) return;

	for (const FMortisAbilityInputBinding& WeaponAbility : DefaultWeaponAbilities)
	{
		if (!WeaponAbility.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(WeaponAbility.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(WeaponAbility.InputTag);

		GrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}

void UMortisAbilitySystemComponent::RemoveGrantedPlayerWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& SpecHandlesToRemove)
{
	if (SpecHandlesToRemove.IsEmpty()) return;

	for (const FGameplayAbilitySpecHandle& SpecHandle : SpecHandlesToRemove)
	{
		if (SpecHandle.IsValid()) ClearAbility(SpecHandle);
	}

	SpecHandlesToRemove.Empty();
}

// Shared
bool UMortisAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*>  FoundAbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty())
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);

		if (!SpecToActivate->IsActive())
			return TryActivateAbility(SpecToActivate->Handle);
	}

	return false;
}
