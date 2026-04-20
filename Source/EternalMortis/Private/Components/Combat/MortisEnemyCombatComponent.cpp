// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Combat/MortisEnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MortisDebugHelper.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Character/Enemy/MortisEnemyData.h"
#include "Character/Player/MortisPlayerCharacter.h"
#include "Items/Weapons/MortisEnemyWeapon.h"

void UMortisEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	Super::OnHitTargetActor(HitActor);
	
	if (!AddUniqueOverlappedActor(HitActor))
	{
		return;
	}
	
	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;
	
	if (AMortisPlayerCharacter* Player = Cast<AMortisPlayerCharacter>(HitActor))
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(), MortisGameplayTags::Event_Combat_AttackHit, EventData);
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, MortisGameplayTags::Event_Action_ImpactReact, EventData);	
	}
}

void UMortisEnemyCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	Super::OnWeaponPulledFromTargetActor(InteractedActor);
}

void UMortisEnemyCombatComponent::OnShieldBeginBlock(AActor* Weapon)
{
	Super::OnShieldBeginBlock(Weapon);
	
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
	
	// MORTIS_LOG("Send Gameplay Event To %s", *Attacker->GetActorNameOrLabel());
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(), MortisGameplayTags::Event_Combat_Block_Success, EventData);
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Attacker, MortisGameplayTags::Event_Combat_Attack_Blocked, EventData);
}

void UMortisEnemyCombatComponent::OnShieldEndBlock(AActor* Weapon)
{
	Super::OnShieldEndBlock(Weapon);
}

FMortisWeaponCommonData UMortisEnemyCombatComponent::GetUnarmedData() const
{
	if (AMortisEnemyCharacter* EnemyCharacter = Cast<AMortisEnemyCharacter>(GetOwner()))
	{
		if (const UMortisEnemyData* Data = EnemyCharacter->GetEnemyData())
		{
			return Data->UnarmedData;
		}
	}
	return FMortisWeaponCommonData();
}

void UMortisEnemyCombatComponent::SetAttackPattern(const UMortisAttackPatternData* PatternData)
{
	AttackPatternWeights.Empty();
	if (PatternData)
	{
		AttackPatternData = PatternData;
		for (const FMortisAttackPattern& AttackPattern : PatternData->AttackPatterns)
		{
			AttackPatternWeights.Add(AttackPattern.Weight);
		}
	}
}

const FMortisAttackPattern* UMortisEnemyCombatComponent::GetAttackPatternByIndex(int32 Index) const
{
	if (!AttackPatternData || !AttackPatternData->AttackPatterns.IsValidIndex(Index))
	{
		MORTIS_LOG("%d is Invalid index", Index);
		return nullptr;
	}
	return &AttackPatternData->AttackPatterns[Index];
}

AMortisEnemyWeapon* UMortisEnemyCombatComponent::GetCurrentEnemyWeapon() const
{
	return Cast<AMortisEnemyWeapon>(GetCurrentWeapon());
}

int32 UMortisEnemyCombatComponent::SelectAttackPattern(float DistanceToTarget, float AngleToTarget)
{
	if (!AttackPatternData)
	{
		return INDEX_NONE;
	}
	// MORTIS_LOG("");
	TArray<int32> ValidIndices;
	float TotalWeight = 0.f;
	for (int32 i = 0; i < AttackPatternData->AttackPatterns.Num(); i++)
	{
		const FMortisAttackPattern& AttackPattern = AttackPatternData->AttackPatterns[i];
		// MORTIS_LOG("DistanceToTarget: %s, AngleToTarget: %s, RequiredPhase: %s",
		// 	(DistanceToTarget >= AttackPattern.MinRange && DistanceToTarget <= AttackPattern.MaxRange) ? TEXT("true") : TEXT("false"), 
		// 	(AngleToTarget >= AttackPattern.MinAngle && AngleToTarget <= AttackPattern.MaxAngle) ? TEXT("true") : TEXT("false"), 
		// 	AttackPattern.RequiredPhases.HasTagExact(CurrentPhase) ? TEXT("true") : TEXT("false"));
		if (DistanceToTarget >= AttackPattern.MinRange &&
			DistanceToTarget <= AttackPattern.MaxRange &&
			AngleToTarget >= AttackPattern.MinAngle &&
			AngleToTarget <= AttackPattern.MaxAngle &&
			(!AttackPattern.RequiredPhases.IsValid() || AttackPattern.RequiredPhases.HasTagExact(CurrentPhase)))
		{
			// MORTIS_LOG("Add: %d", i);
			ValidIndices.Add(i);
			TotalWeight += AttackPatternWeights[i];
		}
	}

	float Accumulated = 0.f;
	float RandomValue = FMath::FRandRange(0.f, TotalWeight);
	int32 SelectedIndex = INDEX_NONE;
	for (const int32 ValidIndex : ValidIndices)
	{
		Accumulated += AttackPatternWeights[ValidIndex];
		if (Accumulated >= RandomValue)
		{
			AttackPatternWeights[ValidIndex] /= 2.f;
			float MinWeight = AttackPatternData->AttackPatterns[ValidIndex].Weight * 0.1f;
			AttackPatternWeights[ValidIndex] = FMath::Max(MinWeight, AttackPatternWeights[ValidIndex]);
			// MORTIS_LOG("ValidIndex: %d", ValidIndex);
			SelectedIndex = ValidIndex;
			break;
		}
	}
	
	for (int32 i = 0; i < AttackPatternWeights.Num(); i++)
	{
		if (i != SelectedIndex)
		{
			AttackPatternWeights[i] = FMath::Min(AttackPatternWeights[i] * 2.f, AttackPatternData->AttackPatterns[i].Weight);
		}
	}
	return SelectedIndex;
}
