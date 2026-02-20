// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/MortisPlayerCombatComponent.h"
#include "Items/Weapons/MortisPlayerWeapon.h"

AMortisPlayerWeapon* UMortisPlayerCombatComponent::GetPlayerCarriedWeaponByTag(FGameplayTag WeaponTag) const
{
	return Cast<AMortisPlayerWeapon>(GetCharacterCarriedWeaponByTag(WeaponTag));
}

AMortisPlayerWeapon* UMortisPlayerCombatComponent::GetPlayerCurrentEquippedWeapon() const
{
	return Cast<AMortisPlayerWeapon>(GetCharacterCurrentEquippedWeapon());
}

void UMortisPlayerCombatComponent::OnHitTargetActor(AActor* HitActor)
{
}

void UMortisPlayerCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
}
