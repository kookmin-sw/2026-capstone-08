// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/MortisPlayerCombatComponent.h"
#include "Character/Player/MortisPlayerCharacter.h"
#include "Items/Weapons/MortisPlayerWeapon.h"
#include "Components/UI/MortisPlayerUIComponent.h"
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

void UMortisPlayerCombatComponent::SetPotionCount(int32 NewPotionCount)
{
	PotionCount = FMath::Max(0, NewPotionCount);
	BroadcastPotionCountChanged();
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

void UMortisPlayerCombatComponent::BroadcastPotionCountChanged() const
{
	const AMortisPlayerCharacter* PlayerCharacter = Cast<AMortisPlayerCharacter>(GetOwner());
	if (!PlayerCharacter)
	{
		return;
	}

	if (UMortisPlayerUIComponent* PlayerUIComponent = PlayerCharacter->GetPlayerUIComponent())
	{
		PlayerUIComponent->OnPotionCountChanged.Broadcast(PotionCount);
	}
}
