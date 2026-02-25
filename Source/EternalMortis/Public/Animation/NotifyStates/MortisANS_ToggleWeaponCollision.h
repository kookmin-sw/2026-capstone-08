// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MortisANS_ToggleWeaponCollision.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisANS_ToggleWeaponCollision : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	void ToggleWeaponCollisionForMesh(const USkeletalMeshComponent* MeshComp, bool bEnable) const;
};
