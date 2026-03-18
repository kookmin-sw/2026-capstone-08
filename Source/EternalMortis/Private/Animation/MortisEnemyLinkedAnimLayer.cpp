// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MortisEnemyLinkedAnimLayer.h"

#include "Animation/MortisEnemyAnimInstance.h"
#include "Character/Enemy/MortisEnemyCharacter.h"

UMortisEnemyAnimInstance* UMortisEnemyLinkedAnimLayer::GetEnemyAnimInstance() const
{
	if (GetOwningComponent())
	{
		return Cast<UMortisEnemyAnimInstance>(GetOwningComponent()->GetAnimInstance());
	}
	return nullptr;
}