// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MortisGameplayAbility.generated.h"

class UMortisCombatComponent;

// 찬하 말대로 이건 Ability를 사용하는 경우에만 필요한 ENUM이라서, 추후에 StructTypes를 세분화 하거나 그냥 MortisStructTypes에 포함시킬까 생각중이다.
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

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UMortisCombatComponent* GetMortisCombatComponentFromActorInfo() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "MortisAbility")
	EMortisAbilityActivationPolicy AbilityActivationPolicy = EMortisAbilityActivationPolicy::OnTriggered;
};
