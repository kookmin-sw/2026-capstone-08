// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/MortisPlayerCombatComponent.h"
#include "Items/Weapons/MortisPlayerWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "MortisGameplayTags.h"

#include "MortisDebugHelper.h"

AMortisPlayerWeapon* UMortisPlayerCombatComponent::GetPlayerCarriedWeaponByTag(FGameplayTag WeaponTag) const
{
	return Cast<AMortisPlayerWeapon>(GetCharacterCarriedWeaponByTag(WeaponTag));
}

AMortisPlayerWeapon* UMortisPlayerCombatComponent::GetPlayerCurrentEquippedWeapon() const
{
	return Cast<AMortisPlayerWeapon>(GetCharacterCurrentEquippedWeapon());
}

FMortisPlayerWeaponData UMortisPlayerCombatComponent::GetPlayerCurrentWeaponData() const
{
	if (!GetPlayerCurrentEquippedWeapon())
	{
		MORTIS_LOG("Current Weapon is invalid!");
		return FMortisPlayerWeaponData();
	}
	return GetPlayerCurrentEquippedWeapon()->PlayerWeaponData;
}

void UMortisPlayerCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (!AddUniqueOverlappedActor(HitActor))
	{
		return;
	}
    FGameplayEventData Data;
    Data.Instigator = GetOwningPawn();
    Data.Target = HitActor;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
        GetOwningPawn(),
        MortisGameplayTags::Event_Combat_AttackHit,
        Data
    );
}

void UMortisPlayerCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{

}
