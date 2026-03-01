// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MortisGameplayAbility.generated.h"

class AMortisCharacterBase;
class UMotionWarpingComponent;
class UMortisAbilitySystemComponent;
class UMortisCombatComponent;

// ���� ����� �̰� Ability�� ����ϴ� ��쿡�� �ʿ��� ENUM�̶�, ���Ŀ� StructTypes�� ����ȭ �ϰų� �׳� MortisStructTypes�� ���Խ�ų�� �������̴�.
UENUM(BlueprintType)
enum class EMortisAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};

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

	UFUNCTION(BlueprintPure, Category = "Mortis|Ability")
	UMortisCombatComponent* GetMortisCombatComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Mortis|Ability")
	UMortisAbilitySystemComponent* GetMortisAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Mortis|Ability")
	AMortisCharacterBase* GetMortisCharacterFromActorInfo() const;
	
	UFUNCTION(BlueprintPure, Category = "Mortis")
	UMotionWarpingComponent* GetMotionWarpingComponent() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "MortisAbility")
	EMortisAbilityActivationPolicy AbilityActivationPolicy = EMortisAbilityActivationPolicy::OnTriggered;
};
