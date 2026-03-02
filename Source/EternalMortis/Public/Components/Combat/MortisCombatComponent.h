// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"
#include "MortisCombatComponent.generated.h"

class AMortisWeaponBase;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	void RegisterSpawnedWeapon(FGameplayTag WeaponTag, AMortisWeaponBase* WeaponToRegister, bool bRegisterAsEquippedWeapon);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	AMortisWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	UPROPERTY(BlueprintReadWrite, Category = "Mortis|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	AMortisWeaponBase* GetCharacterCurrentEquippedWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	void ToggleWeaponCollision(bool bShouldEnable);

	// 아래의 두 Functions은 상속한 CombatComponent에서 구현하기
	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);

	/* Attack Trace */
	void BeginAttackTrace(FName SocketName, float Radius);
	void UpdateAttackTrace();
	void EndAttackTrace();
	
protected:
	virtual void ToggleCurrentEquippedWeaponCollision(bool bShouldEnable);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis")
	TArray<AActor*> OverlappedActors;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mortis|Combat")
	TMap<FGameplayTag, AMortisWeaponBase*> CharacterCarriedWeaponMap;

	/* Attack Trace */
	FName CurrentTraceSocket;
	float CurrentTraceRadius;
	FVector PreviousLocation;
	bool bIsTracing = false;
};
