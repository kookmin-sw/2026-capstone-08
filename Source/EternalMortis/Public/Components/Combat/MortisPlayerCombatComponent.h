// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/MortisCombatComponent.h"
#include "MortisPlayerCombatComponent.generated.h"

class AMortisPlayerWeapon;
struct FMortisPlayerWeaponData;

UCLASS()
class ETERNALMORTIS_API UMortisPlayerCombatComponent : public UMortisCombatComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	AMortisPlayerWeapon* GetPlayerCarriedWeaponByTag(FGameplayTag WeaponTag) const;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	AMortisPlayerWeapon* GetPlayerCurrentEquippedWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	FMortisPlayerWeaponData GetPlayerCurrentWeaponData() const;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	void SetPotionCount(int32 NewPotionCount);

	virtual void OnHitTargetActor(AActor* HitActor) override;
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor) override;

	UPROPERTY(BlueprintReadWrite, Category = "Mortis|Combat")
	int32 PotionCount = 5;

private:
	void BroadcastPotionCountChanged() const;
};
