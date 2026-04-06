// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Types/MortisEnumTypes.h"
#include "MortisGameplayAbility.generated.h"

class AMortisCharacterBase;
class UMotionWarpingComponent;
class UMortisAbilitySystemComponent;
class UMortisCombatComponent;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	// UGameplayAbility Interface
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(const AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability", meta = (DisplayName = "Apply Gameplay Effect Spec Handle To Target Actor", ExpandEnumAsExecs = "OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle, EMortisSuccessType& OutSuccessType);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability")
	void ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& SpecHandle, const TArray<FHitResult>& HitResults);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Ability")
	FGameplayEffectSpecHandle MakeEffectSpecWithSetByCaller(TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag DataTag, float Value, float EffectLevel = 1.f) const;


	UFUNCTION(BlueprintPure, Category = "Mortis|Ability")
	UMortisCombatComponent* GetMortisCombatComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Mortis|Ability")
	UMortisAbilitySystemComponent* GetMortisAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Mortis|Ability")
	AMortisCharacterBase* GetMortisCharacterFromActorInfo() const;
	
	UFUNCTION(BlueprintPure, Category = "Mortis")
	UMotionWarpingComponent* GetMotionWarpingComponent() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mortis|Ability")
	EMortisAbilityActivationPolicy AbilityActivationPolicy = EMortisAbilityActivationPolicy::OnTriggered;
};
