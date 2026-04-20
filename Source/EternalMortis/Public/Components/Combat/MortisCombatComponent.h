// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"
#include "Items/Weapons/MortisShieldBase.h"
#include "Types/MortisStructTypes.h"
#include "MortisCombatComponent.generated.h"

class AMortisWeaponBase;

// 무기, 신체 구분 후 따로 처리
// 상세한 부위는 Tag로 처리하면 됨
UENUM(BlueprintType)
enum class EToggleCollisionType : uint8
{
	CurrentWeapon,
	Body,
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
	void RegisterWeapon(FGameplayTag WeaponTag, AMortisWeaponBase* WeaponToRegister, bool bRegisterAsEquippedWeapon);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	void RegisterWeaponToSlot(FGameplayTag SlotTag, AMortisWeaponBase* WeaponToRegister, bool bRegisterAsEquippedWeapon);
	
	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	bool UnregisterSpawnedWeapon(FGameplayTag WeaponTag);
	
	bool UnRegisterWeapon(const FGameplayTag& SlotTag);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	AMortisWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag TagToGet) const;

	UPROPERTY(BlueprintReadWrite, Category = "Mortis|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	AMortisWeaponBase* GetCharacterCurrentEquippedWeapon() const;
	
	FORCEINLINE AMortisWeaponBase* GetEquippedWeaponBySlotTag(const FGameplayTag& SlotTag) const { return WeaponBySlotTag.FindRef(SlotTag); }
	
	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat")
	void ToggleDamageCollision(bool bShouldEnable, FGameplayTag TagToToggle, EToggleCollisionType ToggleDamageType = EToggleCollisionType::CurrentWeapon);

	void ClearWeapons(float LifeSpan = 0.f);
	
	// 아래의 두 Functions은 상속한 CombatComponent에서 구현하기
	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);

	/* Shield Callbacks */
	virtual void OnShieldBeginBlock(AActor* Weapon);
	virtual void OnShieldEndBlock(AActor* Weapon);
	
	/* Attack Trace */
	UPROPERTY(EditDefaultsOnly, Category = "Mortis|AttackTrace")
	TArray<TEnumAsByte<EObjectTypeQuery>> AttackTraceObjectTypes;
	
	UPROPERTY()
	TArray<FMortisAttackTraceConfig> AttackTraceConfigs;
	
	void BeginAttackTrace(const FMortisAttackTraceConfig& Config);
	void UpdateAttackTrace();
	void EndAttackTrace(const FMortisAttackTraceConfig& Config);
	
	FORCEINLINE void SetCurrentSlotTag(const FGameplayTag& NewSlotTag) { CurrentAttackSlot = NewSlotTag; }
	AMortisWeaponBase* GetCurrentWeapon() const;
protected:
	virtual void ToggleCurrentEquippedWeaponCollision(bool bShouldEnable, FGameplayTag TagToToggle);
	virtual void ToggleBodyDamageCollision(bool bShouldEnable, FGameplayTag TagToToggle);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|AttackTrace")
	TArray<TObjectPtr<AActor>> OverlappedActors;
	
	bool AddUniqueOverlappedActor(AActor* NewActor);
	FORCEINLINE void ClearOverlappedActors() { OverlappedActors.Empty(); }
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Mortis|Combat")
	TMap<FGameplayTag, TObjectPtr<AMortisWeaponBase>> CharacterWeaponMap;

	UPROPERTY(VisibleAnywhere, Category = "Mortis|Combat")
	TMap<FGameplayTag, TObjectPtr<AMortisWeaponBase>> WeaponBySlotTag;
	
	FGameplayTag CurrentAttackSlot = FGameplayTag::EmptyTag;
	
	/* Attack Trace */
	FName CurrentTraceSocket;
	TArray<FVector> PreviousTraceStartLocations;
	TArray<FVector> PreviousTraceEndLocations;
	bool bIsTracing = false;
	
	FVector GetSocketLocation(const FMortisAttackTraceConfig& Config, bool bStart) const;
};
