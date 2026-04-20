// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/MortisCombatComponent.h"
#include "Types/MortisStructTypes.h"
#include "MortisEnemyCombatComponent.generated.h"

class AMortisEnemyWeapon;
class UMortisAttackPatternData;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisEnemyCombatComponent : public UMortisCombatComponent
{
	GENERATED_BODY()

public:
	//~ Begin UMortisCombatComponent Intferface
	virtual void OnHitTargetActor(AActor* HitActor) override;
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor) override;
	
	virtual void OnShieldBeginBlock(AActor* Weapon) override;
	virtual void OnShieldEndBlock(AActor* Weapon) override;
	//~ End UMortisCombatComponent Intferface
	
	FMortisWeaponCommonData GetUnarmedData() const;
protected:
	UPROPERTY()
	TObjectPtr<const UMortisAttackPatternData> AttackPatternData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|AttackPattern")
	int32 CurrentAttackPatternIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|AttackPattern")
	int32 PrevAttackPatternIndex = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|AttackPattern")
	FGameplayTag CurrentPhase = MortisGameplayTags::State_Enemy_Phase_1;
	
public:
	void SetAttackPattern(const UMortisAttackPatternData* PatternData);
	
	FORCEINLINE const FMortisAttackPattern* GetAttackPatternByIndex(int32 Index) const;

	FORCEINLINE AMortisEnemyWeapon* GetCurrentEnemyWeapon() const;
	int32 SelectAttackPattern(float DistanceToTarget, float AngleToTarget);
	
private:
	TArray<float> AttackPatternWeights;
};
