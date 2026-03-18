// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MortisAnimInstanceBase.h"
#include "MortisFunctionLibrary.h"

bool UMortisAnimInstanceBase::DoesOwnerHaveTag(FGameplayTag GameplayTag) const
{
	if (APawn* OwningPawn = TryGetPawnOwner())
	{
		return UMortisFunctionLibrary::HasGameplayTag(OwningPawn, GameplayTag);
	}
	return false;
}
