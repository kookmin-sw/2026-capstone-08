// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/MortisEnumTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "MortisAT_UpdateWarpTarget.generated.h"

class UMotionWarpingComponent;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisAT_UpdateWarpTarget : public UAbilityTask
{
	GENERATED_BODY()

public:
	/*
	 * @Param WarpUpdateDuration default value is -1. Warp update for anim montage 
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability|Task", meta = (HidePin = "OwningAbility", DefaultToOwningAbility = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UMortisAT_UpdateWarpTarget* UpdateWarpTarget(UGameplayAbility* OwningAbility, FName WarpTargetName, AActor* TargetActor, EMortisWarpTargetMode WarpTargetMode, float
		DesiredDistance, float UpdateInterval = 0.1f);

public:
	//~ Begin UGameplayTask Interface
	virtual void TickTask(float DeltaTime) override;
	//~ End UGameplayTask Interface
	
protected:
	//~ Begin UGameplayTask Interface
	virtual void Activate() override;
	//~ End UGameplayTask Interface

private:
	void UpdateTarget();
	// FVector CalculateWarpTargetLocation();
	FName WarpTargetName;
	EMortisWarpTargetMode WarpTargetMode;
	float DesiredDistance = 0.f;
	
	float UpdateInterval = 0.1f;
	
	float TimeSinceLastUpdate = 0.1f;

	bool bIsUpdateFinished = false;
	
	TWeakObjectPtr<AActor> CachedTarget;
	TWeakObjectPtr<UMotionWarpingComponent> CachedMWC;
};
