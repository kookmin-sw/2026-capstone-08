// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/MortisCombatComponent.h"
#include "Items/Weapons/MortisWeaponBase.h"
#include "Components/BoxComponent.h"


void UMortisCombatComponent::RegisterSpawnedWeapon(FGameplayTag WeaponTag, AMortisWeaponBase* WeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	checkf(!CharacterCarriedWeaponMap.Contains(WeaponTag), TEXT("A weapon named %s has already been added as carried weapon"), *WeaponTag.ToString());
	check(WeaponToRegister);

	CharacterCarriedWeaponMap.Emplace(WeaponTag, WeaponToRegister);
	WeaponToRegister->OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
	WeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTargetActor);

	if (bRegisterAsEquippedWeapon)
		CurrentEquippedWeaponTag = WeaponTag;
}

AMortisWeaponBase* UMortisCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag WeaponTagToGet) const
{
	if (CharacterCarriedWeaponMap.Contains(WeaponTagToGet))
	{
		if (AMortisWeaponBase* const* FoundWeapon = CharacterCarriedWeaponMap.Find(WeaponTagToGet))
			return *FoundWeapon;
	}
	return nullptr;
}

AMortisWeaponBase* UMortisCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if (!CurrentEquippedWeaponTag.IsValid()) return nullptr;
	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

void UMortisCombatComponent::ToggleDamageCollision(bool bShouldEnable, FGameplayTag TagToToggle, EToggleCollisionType ToggleDamageType)
{
	checkf(TagToToggle.IsValid(), TEXT("TagToToggle Is Not Valid! Check AnimNotifyState On Your Attack Animation!"))
	if (ToggleDamageType == EToggleCollisionType::CurrentWeapon)
		ToggleCurrentEquippedWeaponCollision(bShouldEnable, TagToToggle);
	else
		ToggleBodyDamageCollision(bShouldEnable, TagToToggle);
}

void UMortisCombatComponent::ToggleCurrentEquippedWeaponCollision(bool bShouldEnable, FGameplayTag TagToToggle)
{
	AMortisWeaponBase* WeaponToToggle = GetCharacterCurrentEquippedWeapon();
	check(WeaponToToggle);
	if (bShouldEnable)
		WeaponToToggle->GetWeaponCollisionBox(TagToToggle)->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	else
	{
		WeaponToToggle->GetWeaponCollisionBox(TagToToggle)->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		OverlappedActors.Empty();
	}
}

void UMortisCombatComponent::ToggleBodyDamageCollision(bool bShouldEnable, FGameplayTag TagToToggle)
{

}

// 자식에서 Override
void UMortisCombatComponent::OnHitTargetActor(AActor* HitActor)
{
}
void UMortisCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
}
