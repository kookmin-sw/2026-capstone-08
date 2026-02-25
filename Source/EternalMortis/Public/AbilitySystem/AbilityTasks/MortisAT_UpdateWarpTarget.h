// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability|Task", meta = (HidePin = "OwningAbility", DefaultToOwningAbility = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UMortisAT_UpdateWarpTarget* UpdateWarpTarget(UGameplayAbility* OwningAbility, FName WarpTargetName, AActor* TargetActor, float UpdateInterval = 0.1f);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void UpdateTarget();

	FName WarpTargetName;
	float UpdateInterval;
	
	TWeakObjectPtr<AActor> CachedTarget;
	TWeakObjectPtr<UMotionWarpingComponent> CachedMWC;

	FTimerHandle TimerHandle;	
};
