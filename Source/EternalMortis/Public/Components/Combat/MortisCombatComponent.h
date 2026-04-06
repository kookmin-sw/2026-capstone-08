// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"
#include "Items/Weapons/MortisShieldBase.h"
#include "MortisCombatComponent.generated.h"

class AMortisWeaponBase;

// 무기, 신체 구분 후 따로 처리
// 상세한 부위는 Tag로 처리하면 됨
UENUM(BlueprintType)
enum class EToggleCollisionType : uint8
{
	CurrentWeapon,
	Shield,
	Body
};
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	void RegisterCombatItem(FGameplayTag ItemTag, AMortisCombatItemBase* ItemToRegister, bool bRegisterAsEquippedWeapon);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	bool UnregisterSpawnedWeapon(FGameplayTag WeaponTag);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	AMortisWeaponBase* GetCharacterCarriedItemByTag(FGameplayTag TagToGet) const;

	UPROPERTY(BlueprintReadWrite, Category = "Mortis|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	AMortisWeaponBase* GetCharacterCurrentEquippedWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	void ToggleDamageCollision(bool bShouldEnable, FGameplayTag TagToToggle, EToggleCollisionType ToggleDamageType = EToggleCollisionType::CurrentWeapon);

	// 아래의 두 Functions은 상속한 CombatComponent에서 구현하기
	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);

	/* Shield Callbacks */
	virtual void OnShieldBeginBlock(AActor* Weapon);
	virtual void OnShieldEndBlock(AActor* Weapon);
	
	/* Attack Trace */
	void BeginAttackTrace(FName SocketName, float Radius);
	void UpdateAttackTrace();
	void EndAttackTrace();
	
protected:
	virtual void ToggleCurrentEquippedWeaponCollision(bool bShouldEnable, FGameplayTag TagToToggle);
	virtual void ToggleBodyDamageCollision(bool bShouldEnable, FGameplayTag TagToToggle);

private:
	UPROPERTY(VisibleAnywhere, Category = "Mortis|Combat")
	TMap<FGameplayTag, AMortisWeaponBase*> CharacterWeaponMap;

	/* Attack Trace */
	FName CurrentTraceSocket;
	float CurrentTraceRadius;
	FVector PreviousLocation;
	bool bIsTracing = false;
};
