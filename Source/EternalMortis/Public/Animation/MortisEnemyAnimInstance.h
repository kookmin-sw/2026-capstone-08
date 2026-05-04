// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/MortisStructTypes.h"

#include "GameplayEffectTypes.h"
#include "Animation/MortisCharacterAnimInstanceBase.h"
#include "MortisEnemyAnimInstance.generated.h"

class AMortisAIController;
class AMortisEnemyWeapon;
class AMortisEnemyCharacter;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisEnemyAnimInstance : public UMortisCharacterAnimInstanceBase
{
	GENERATED_BODY()

public:
	//~ Begin UAnimInstance Interfaces
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	//~ End UAnimInstance Interfaces
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData")
	float AimYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData")
	float AimPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData")
	float YawDelta;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData")
	float AimOffsetInterpSpeed = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mortls|Mappping")
	TArray<FMortisTagToBoolMapping> TagToBoolMappings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData")
	bool bIsChasing;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData")
	bool bIsStrafing;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData")
	bool bIsGuarding;
	
private:
	void UpdateAimOffset(float DeltaSeconds);
	void BindTagMappings(UAbilitySystemComponent* ASC);
	void OnTagChanged(const FGameplayTag Tag, int32 NewCount, FName BoolPropertyName);

	FRotator PrevRotation;
};
