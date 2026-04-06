// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MortisPlayerLinkedAnimLayer.h"
#include "Animation/MortisPlayerAnimInstance.h"

UMortisPlayerAnimInstance* UMortisPlayerLinkedAnimLayer::GetPlayerAnimInstance() const
{
	return Cast<UMortisPlayerAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
