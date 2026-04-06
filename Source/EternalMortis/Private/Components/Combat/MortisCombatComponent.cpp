// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/MortisCombatComponent.h"
#include "MortisDebugHelper.h"
#include "Items/Weapons/MortisWeaponBase.h"
#include "Items/Weapons/MortisShieldBase.h"

#include "Components/BoxComponent.h"


void UMortisCombatComponent::RegisterCombatItem(FGameplayTag ItemTag, AMortisCombatItemBase* ItemToRegister, bool bRegisterAsEquippedWeapon)
{
	checkf(ItemToRegister, TEXT("ItemToRegister is null for tag: %s"), *ItemTag.ToString());
	checkf(!CharacterWeaponMap.Contains(ItemTag), TEXT("Item with tag %s is already registered"), *ItemTag.ToString());
    
	if (AMortisWeaponBase* Weapon = Cast<AMortisWeaponBase>(ItemToRegister))
	{
		CharacterWeaponMap.Emplace(ItemTag, Weapon);
		
		Weapon->OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
		Weapon->OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTargetActor);
        
		if (bRegisterAsEquippedWeapon)
		{
			CurrentEquippedWeaponTag = ItemTag;
		}
		MORTIS_LOG("Weapon Registered: %s", *ItemTag.ToString());
	}
	else if (AMortisShieldBase* Shield = Cast<AMortisShieldBase>(ItemToRegister))
	{
		Shield->OnShieldBeginBlock.BindUObject(this, &ThisClass::OnShieldBeginBlock);
		Shield->OnShieldEndBlock.BindUObject(this, &ThisClass::OnShieldEndBlock);
		MORTIS_LOG("Shield Registered: %s", *ItemTag.ToString());
	}

}
bool UMortisCombatComponent::UnregisterSpawnedWeapon(FGameplayTag WeaponTag)
{
	if (!WeaponTag.IsValid())
		return false;

	AMortisWeaponBase* Weapon = nullptr;
	if (AMortisWeaponBase* const* FoundWeapon = CharacterWeaponMap.Find(WeaponTag))
	{
		Weapon = *FoundWeapon;
	}

	// 현재 장착 무기 상태 정리
	if (CurrentEquippedWeaponTag == WeaponTag)
	{
		CurrentEquippedWeaponTag = FGameplayTag();
		// OverlappedActors.Empty();
	}

	// delegate 정리
	if (IsValid(Weapon))
	{
		Weapon->OnWeaponHitTarget.Unbind();
		Weapon->OnWeaponPulledFromTarget.Unbind();
	}

	// 맵에서는 무조건 제거
	return CharacterWeaponMap.Remove(WeaponTag) > 0;
}
AMortisWeaponBase* UMortisCombatComponent::GetCharacterCarriedItemByTag(FGameplayTag TagToGet) const
{
	if (CharacterWeaponMap.Contains(TagToGet))
	{
		if (AMortisWeaponBase* const* FoundWeapon = CharacterWeaponMap.Find(TagToGet))
			return *FoundWeapon;
	}
	return nullptr;
}

AMortisWeaponBase* UMortisCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		// MORTIS_LOG("%s is invalid!", *CurrentEquippedWeaponTag.GetTagName().ToString());
		return nullptr;
	}
	return GetCharacterCarriedItemByTag(CurrentEquippedWeaponTag);
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
	if (!TagToToggle.IsValid())
	{
		MORTIS_LOG("%s is invalid!", *TagToToggle.ToString());
		return;
	}
	AMortisWeaponBase* WeaponToToggle = GetCharacterCurrentEquippedWeapon();
	if (!WeaponToToggle)
	{
		MORTIS_LOG("WeaponToToggle is null!");
		return;
	}
	UShapeComponent* CollisionShape = WeaponToToggle->GetCollisionComponentByTag(TagToToggle);
	if (!CollisionShape)
	{
		MORTIS_LOG("ShapeComponent is null!");
		return;
	}
	
	if (bShouldEnable)
	{
		CollisionShape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		CollisionShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponToToggle->ClearOverlappedActors();
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

void UMortisCombatComponent::OnShieldBeginBlock(AActor* Weapon)
{
}

void UMortisCombatComponent::OnShieldEndBlock(AActor* Weapon)
{
}
