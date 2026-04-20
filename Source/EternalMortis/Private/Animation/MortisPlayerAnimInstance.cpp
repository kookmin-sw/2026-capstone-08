// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MortisPlayerAnimInstance.h"
#include "Character/Player/MortisPlayerCharacter.h"

void UMortisPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwningCharacter)
		OwningPlayerCharacter = Cast<AMortisPlayerCharacter>(OwningCharacter);
}
