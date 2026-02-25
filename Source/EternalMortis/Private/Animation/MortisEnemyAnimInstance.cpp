// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MortisEnemyAnimInstance.h"

#include "MortisDebugHelper.h"

void UMortisEnemyAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	
	if (UAnimMontage* CurrentMontage = GetCurrentActiveMontage())
	{
		float CurveSpeed = GetCurveValue(FName("PlayRateCurve"));
		if (CurveSpeed > 0.0f)
		{
			Montage_SetPlayRate(CurrentMontage, CurveSpeed);
		}
	}
}
