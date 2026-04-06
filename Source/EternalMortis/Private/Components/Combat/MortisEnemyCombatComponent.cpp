// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Combat/MortisEnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MortisDebugHelper.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Character/Enemy/MortisEnemyData.h"
#include "Items/Weapons/MortisEnemyWeapon.h"

void UMortisEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	Super::OnHitTargetActor(HitActor);
	
	MORTIS_LOG("OnHitTargetActor");
	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(), MortisGameplayTags::Event_Combat_AttackHit, EventData);
}

void UMortisEnemyCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	Super::OnWeaponPulledFromTargetActor(InteractedActor);
}

void UMortisEnemyCombatComponent::OnShieldBeginBlock(AActor* Weapon)
{
	Super::OnShieldBeginBlock(Weapon);
	
	MORTIS_LOG("");
	AActor* WeaponActor = Weapon->GetOwner();
	if (!WeaponActor)
	{
		return;
	}
	
	APawn* Attacker = WeaponActor->GetInstigator();
	if (!Attacker)
	{
		return;
	}
	
	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = Attacker;
	
	MORTIS_LOG("Send Gameplay Event To %s", *Attacker->GetActorNameOrLabel());
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(), MortisGameplayTags::Event_Combat_Block_Success, EventData);
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Attacker, MortisGameplayTags::Event_Combat_Attack_Blocked, EventData);
}

void UMortisEnemyCombatComponent::OnShieldEndBlock(AActor* Weapon)
{
	Super::OnShieldEndBlock(Weapon);
}

void UMortisEnemyCombatComponent::SetAttackPattern(UMortisAttackPatternData* PatternData)
{
	if (PatternData)
	{
		// MORTIS_LOG("");
		AttackPatterns = PatternData->AttackPatterns;
	}
}

const FMortisAttackPattern* UMortisEnemyCombatComponent::GetAttackPatternByIndex(int32 Index) const
{
	if (!AttackPatterns.IsValidIndex(Index))
	{
		MORTIS_LOG("%d is Invalid index", Index);
		return nullptr;
	}
	return &AttackPatterns[Index];
}

AMortisEnemyWeapon* UMortisEnemyCombatComponent::GetEnemyWeapon() const
{
	return Cast<AMortisEnemyWeapon>(GetCharacterCurrentEquippedWeapon());
}

int32 UMortisEnemyCombatComponent::SelectAttackPattern(float DistanceToTarget, float AngleToTarget) const
{
	// MORTIS_LOG("");
	TArray<int32> ValidIndices;
	float TotalWeight = 0.f;
	for (int32 i = 0; i < AttackPatterns.Num(); i++)
	{
		MORTIS_LOG("DistanceToTarget: %s, AngleToTarget: %s, RequiredPhase: %s",
			(DistanceToTarget >= AttackPatterns[i].MinRange && DistanceToTarget <= AttackPatterns[i].MaxRange) ? TEXT("true") : TEXT("false"), 
			(AngleToTarget >= AttackPatterns[i].MinAngle && AngleToTarget <= AttackPatterns[i].MaxAngle) ? TEXT("true") : TEXT("false"), 
			AttackPatterns[i].RequiredPhases.HasTagExact(CurrentPhase) ? TEXT("true") : TEXT("false"));
		if (DistanceToTarget >= AttackPatterns[i].MinRange &&
			DistanceToTarget <= AttackPatterns[i].MaxRange &&
			AngleToTarget >= AttackPatterns[i].MinAngle &&
			AngleToTarget <= AttackPatterns[i].MaxAngle &&
			(!AttackPatterns[i].RequiredPhases.IsValid() || AttackPatterns[i].RequiredPhases.HasTagExact(CurrentPhase)))
		{
			// MORTIS_LOG("Add: %d", i);
			ValidIndices.Add(i);
			TotalWeight += AttackPatterns[i].Weight;
		}
	}

	float Accumulated = 0.f;
	float RandomValue = FMath::FRandRange(0.f, TotalWeight);
	for (const int32 ValidIndex : ValidIndices)
	{
		Accumulated += AttackPatterns[ValidIndex].Weight;
		if (Accumulated >= RandomValue)
		{
			return ValidIndex;
		}
	}
	return INDEX_NONE;
}
