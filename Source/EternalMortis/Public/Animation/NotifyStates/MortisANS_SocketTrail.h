// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Types/MortisEnumTypes.h"
#include "MortisANS_SocketTrail.generated.h"

class AMortisWeaponBase;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisANS_SocketTrail : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	//~ Begin UAnimNotifyState Interface
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	//~ End UAnimNotifyState Interface
	
protected:
	UPROPERTY(EditAnywhere, Category = "Mortis|Trail")
	EMortisMeshSource MeshSource = EMortisMeshSource::WeaponMesh;
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Trail")
	FName TrailStartSocketName = TEXT("TrailStart");
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Trail")
	FName TrailEndSocketName = TEXT("TrailEnd");
	
	UPROPERTY(EditAnywhere, Category = "Mortis|Trail")
	float TrailWidth = 5.f;
	
private:
	void ToggleTrailFromCharacterMesh(USkeletalMeshComponent* MeshComp, bool bEnable) const;
};
