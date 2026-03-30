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
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData")
	float AimYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData")
	float AimPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData")
	float AimOffsetInterpSpeed = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mortls|Mappping")
	TArray<FMortisTagToBoolMapping> TagToBoolMappings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData")
	bool bIsChasing;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData")
	bool bIsStrafing;
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData|IK")
    float CurrentIKAlpha = 0.f;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortls|AnimData|IK")
    FTransform LeftHandIKTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|IK")
	FVector LeftElbowJointTarget;

	UPROPERTY(EditAnywhere, Category = "AnimData|IK")
	float ElbowOutOffset = 50.f;

	UPROPERTY(EditAnywhere, Category = "AnimData|IK")
	float ElbowBackOffset = 30.f;
	
private:
	void UpdateAimOffset(float DeltaSeconds);
	void UpdateIKAlpha(float DeltaSeconds);
	void UpdateLeftElbowJointTarget();
	void BindTagMappings(UAbilitySystemComponent* ASC);
	void OnTagChanged(const FGameplayTag Tag, int32 NewCount, FName BoolPropertyName);

	bool bUseTwoHandedIK = false;
	FName LeftHandGripSocketName = NAME_None;
	FName LeftUpperArmSocketName = TEXT("upperarm_l");
};
