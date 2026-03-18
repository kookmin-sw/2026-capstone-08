// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/MortisStructTypes.h"

#include "GameplayEffectTypes.h"
#include "Animation/MortisCharacterAnimInstanceBase.h"
#include "MortisEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisEnemyAnimInstance : public UMortisCharacterAnimInstanceBase
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Mappping")
	TArray<FMortisTagToBoolMapping> TagToBoolMappings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimData")
	bool bIsStrafing;
	
private:
	void BindTagMappings(UAbilitySystemComponent* ASC);
	void OnTagChanged(const FGameplayTag Tag, int32 NewCount, FName BoolPropertyName);
};
