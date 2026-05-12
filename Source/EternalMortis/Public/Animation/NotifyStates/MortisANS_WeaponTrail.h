// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Components/Combat/MortisCombatComponent.h"
#include "Items/Weapons/MortisWeaponBase.h"
#include "Types/MortisEnumTypes.h"
#include "MortisANS_WeaponTrail.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisANS_WeaponTrail : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	// UAnimNotifyState Override
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation) override;

private:
	AMortisWeaponBase* GetEquippedWeapon(USkeletalMeshComponent* MeshComp) const;
};
