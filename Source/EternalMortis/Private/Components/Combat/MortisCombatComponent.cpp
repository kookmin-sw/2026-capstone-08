// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/MortisCombatComponent.h"
#include "MortisDebugHelper.h"
#include "Items/Weapons/MortisWeaponBase.h"
#include "Items/Weapons/MortisShieldBase.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"


void UMortisCombatComponent::RegisterWeapon(FGameplayTag WeaponTag, AMortisWeaponBase* WeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	checkf(WeaponToRegister, TEXT("ItemToRegister is null for tag: %s"), *WeaponTag.ToString());
	checkf(!CharacterWeaponMap.Contains(WeaponTag), TEXT("Item with tag %s is already registered"), *WeaponTag.ToString());
    
	if (AMortisWeaponBase* Weapon = Cast<AMortisWeaponBase>(WeaponToRegister))
	{
		CharacterWeaponMap.Emplace(WeaponTag, Weapon);
		
		Weapon->OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
		Weapon->OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTargetActor);
        
		if (bRegisterAsEquippedWeapon)
		{
			CurrentEquippedWeaponTag = WeaponTag;
		}
		// MORTIS_LOG("Weapon Registered: %s", *ItemTag.ToString());
		
		if (AMortisShieldBase* Shield = Cast<AMortisShieldBase>(WeaponToRegister))
		{
			Shield->OnShieldBeginBlock.BindUObject(this, &ThisClass::OnShieldBeginBlock);
			Shield->OnShieldEndBlock.BindUObject(this, &ThisClass::OnShieldEndBlock);
			// MORTIS_LOG("Shield Registered: %s", *ItemTag.ToString());
		}
	}

}
bool UMortisCombatComponent::UnregisterSpawnedWeapon(FGameplayTag WeaponTag)
{
	if (!WeaponTag.IsValid())
		return false;

	AMortisWeaponBase* Weapon = nullptr;
	if (TObjectPtr<AMortisWeaponBase> const* FoundWeapon = CharacterWeaponMap.Find(WeaponTag))
	{
		Weapon = *FoundWeapon;
	}

	// 현재 장착 무기 상태 정리
	if (CurrentEquippedWeaponTag == WeaponTag)
	{
		CurrentEquippedWeaponTag = FGameplayTag();
		OverlappedActors.Empty();
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
AMortisWeaponBase* UMortisCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag TagToGet) const
{
	if (CharacterWeaponMap.Contains(TagToGet))
	{
		if (TObjectPtr<AMortisWeaponBase> const* FoundWeapon = CharacterWeaponMap.Find(TagToGet))
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
	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

void UMortisCombatComponent::ToggleDamageCollision(bool bShouldEnable, FGameplayTag TagToToggle, EToggleCollisionType ToggleDamageType)
{
	checkf(TagToToggle.IsValid(), TEXT("TagToToggle Is Not Valid! Check AnimNotifyState On Your Attack Animation!"))
	
	switch (ToggleDamageType)
	{
	case EToggleCollisionType::CurrentWeapon:
		ToggleCurrentEquippedWeaponCollision(bShouldEnable, TagToToggle);
		break;
	case EToggleCollisionType::Body:
		ToggleBodyDamageCollision(bShouldEnable, TagToToggle);
		break;
	}
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
	
	WeaponToToggle->ToggleCollision(bShouldEnable, TagToToggle);

	if (!bShouldEnable)
	{
		OverlappedActors.Empty();
	}
}

void UMortisCombatComponent::ToggleBodyDamageCollision(bool bShouldEnable, FGameplayTag TagToToggle)
{

}

bool UMortisCombatComponent::AddUniqueOverlappedActor(AActor* NewActor)
{
	if (NewActor && !OverlappedActors.Contains(NewActor))
	{
		OverlappedActors.Add(NewActor);
		return true;
	}
	return false;
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

/* Attack Trace */
void UMortisCombatComponent::BeginAttackTrace(const FMortisAttackTraceConfig& Config)
{
	AttackTraceConfigs.Add(Config);
	PreviousTraceStartLocations.Add(GetSocketLocation(Config, true));
	PreviousTraceEndLocations.Add(GetSocketLocation(Config, false));
}

void UMortisCombatComponent::UpdateAttackTrace()
{
	for (int32 i = 0; i < AttackTraceConfigs.Num(); i++)
	{
		const FVector CurrentStart = GetSocketLocation(AttackTraceConfigs[i], true);
		const FVector CurrentEnd = GetSocketLocation(AttackTraceConfigs[i], false);
		
		TArray<FHitResult> HitResults;
		TArray<AActor*> IgnoreActors;
		UKismetSystemLibrary::SphereTraceMultiForObjects(
			this,
			PreviousTraceStartLocations[i],
			CurrentEnd,
			AttackTraceConfigs[i].Radius,
			AttackTraceObjectTypes,
			false,
			IgnoreActors,
			EDrawDebugTrace::ForDuration,
			HitResults,
			true,
			FLinearColor::Green
		);
	
		PreviousTraceStartLocations[i] = CurrentStart;
		PreviousTraceEndLocations[i] = CurrentEnd;
		
		for (const FHitResult& HitResult : HitResults)
		{
			if (AActor* HitActor = HitResult.GetActor())
			{
				OnHitTargetActor(HitActor);
			}
		}
	}
}

void UMortisCombatComponent::EndAttackTrace(const FMortisAttackTraceConfig& Config)
{
	const int32 Index = AttackTraceConfigs.IndexOfByKey(Config);
	if (AttackTraceConfigs.IsValidIndex(Index))
	{
		AttackTraceConfigs.RemoveAt(Index);
		PreviousTraceStartLocations.RemoveAt(Index);
		PreviousTraceEndLocations.RemoveAt(Index);
	}
	
	if (AttackTraceConfigs.IsEmpty())
	{
		OverlappedActors.Empty();
	}
}


FVector UMortisCombatComponent::GetSocketLocation(const FMortisAttackTraceConfig& Config, bool bStart) const
{
	if (Config.MeshSource == EMortisMeshSource::WeaponMesh)
	{
		if (AMortisWeaponBase* CurrentWeapon = GetCharacterCurrentEquippedWeapon())
		{
			if (UMeshComponent* Mesh = CurrentWeapon->GetWeaponMesh())
			{
				return Mesh->GetSocketLocation(bStart ? Config.StartSocket : Config.EndSocket);
			}
		}
	}
	else if (Config.MeshSource == EMortisMeshSource::CharacterMesh)
	{
		if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			if (USkeletalMeshComponent* Mesh = Character->GetMesh())
			{
				return Mesh->GetSocketLocation(bStart ? Config.StartSocket : Config.EndSocket);
			}
		}
	}
	return FVector::ZeroVector;
}
