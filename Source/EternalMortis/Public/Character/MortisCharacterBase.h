// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"

#include "Interfaces/MortisCombatInterface.h"
#include "MortisCharacterBase.generated.h"

class UNiagaraComponent;
class UMotionWarpingComponent;
class UMortisAbilitySystemComponent;
class UMortisAttributeSet;
class UMortisAbilitySetBase;


UCLASS()
class ETERNALMORTIS_API AMortisCharacterBase : public ACharacter, public IAbilitySystemInterface, public IMortisCombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMortisCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// IAbilitySystemInterface Override
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// IMortisCombatInterface Override
	virtual UMortisCombatComponent* GetCombatComponent() const override { return nullptr; }
	
	UFUNCTION(BlueprintCallable)
	virtual void StartDeath();
	
	UFUNCTION(BlueprintCallable)
	virtual void FinishDeath();
	
protected:
	// APawn Override
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UMortisAbilitySystemComponent* MortisAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UMortisAttributeSet* MortisAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem")
	TSoftObjectPtr<UMortisAbilitySetBase> CharacterAbilitySet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MotionWarping")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	TObjectPtr<UNiagaraComponent> TrailNiagaraComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	TObjectPtr<UParticleSystemComponent> TrailCascadeComponent;

public:
	FORCEINLINE UMortisAbilitySystemComponent* GetMortisAbilitySystemComponent() const { return MortisAbilitySystemComponent; }
	FORCEINLINE UMortisAttributeSet* GetMortisAttributeSet() const { return MortisAttributeSet; }
	FORCEINLINE UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }
	FORCEINLINE UNiagaraComponent* GetTrailNiagaraComponent() const { return TrailNiagaraComponent; }
	FORCEINLINE UParticleSystemComponent* GetTrailCascadeComponent() const { return TrailCascadeComponent; }
	
	void StartCascadeTrails(FName TrailStartSocketName, FName TrailEndSocketName, float Width);
	void EndCascadeTrail();
};
