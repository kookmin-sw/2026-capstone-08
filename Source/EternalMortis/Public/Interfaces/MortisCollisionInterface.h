// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "MortisCollisionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMortisCollisionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ETERNALMORTIS_API IMortisCollisionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FGameplayTag GetCollisionTag() const = 0;
};
