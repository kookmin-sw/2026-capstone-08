// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MortisEnemyLinkedAnimLayer.h"
#include "MortisKnightLinkedAnimLayer.generated.h"

class AMortisEnemyCharacter;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisKnightLinkedAnimLayer : public UMortisEnemyLinkedAnimLayer
{
	GENERATED_BODY()
	
public:
	//~ Begin UAnimInstance Interfaces
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	//~ End UAnimInstance Interfaces
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortls|AnimData|IK")
	float CurrentIKAlpha = 0.f;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortls|AnimData|IK")
	FTransform LeftHandIKTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Morits|AnimData|IK")
	FVector LeftElbowJointTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Morits|AnimData|IK")
	float ElbowOutOffset = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Morits|AnimData|IK")
	float ElbowBackOffset = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Morits|AnimData|IK")
	FName LeftHandGripSocketName = TEXT("LeftHandGripSocket");
	
	UPROPERTY(EditDefaultsOnly, Category = "Morits|AnimData|IK")
	FName LeftUpperArmSocketName = TEXT("upperarm_l");
	
private:
	UPROPERTY()
	TObjectPtr<AMortisEnemyCharacter> OwningCharacter;
	
	void UpdateIKAlpha(float DeltaSeconds);
	void UpdateLeftElbowJointTarget();
};
