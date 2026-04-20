// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/MortisCharacterAnimInstanceBase.h"
#include "MortisPlayerAnimInstance.generated.h"

class AMortisPlayerCharacter;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisPlayerAnimInstance : public UMortisCharacterAnimInstanceBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|References")
	AMortisPlayerCharacter* OwningPlayerCharacter;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "AnimData|References")
	bool bIsRunning = false;

public:
	// UMortisCharacterAnimInstanceBase override
	virtual void NativeInitializeAnimation() override;
};
