// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyStates/MortisANS_RotateFromCurve.h"

void UMortisANS_RotateFromCurve::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp)
	{
		LastCurveValues.Add(MeshComp, 0.f);
	}
}

void UMortisANS_RotateFromCurve::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}
	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst)
	{
		return;
	}
	float* LastCurveValuePtr = LastCurveValues.Find(MeshComp); 
	if (!LastCurveValuePtr)
	{
		return;
	}
	float CurrentCurveValue = AnimInst->GetCurveValue(CurveName);
	float DeltaRotation = CurrentCurveValue - *LastCurveValuePtr;

	if (!FMath::IsNearlyZero(DeltaRotation))
	{
		MeshComp->GetOwner()->AddActorWorldRotation(FRotator(0.f, DeltaRotation, 0.f));
		*LastCurveValuePtr = CurrentCurveValue;
	}
}

void UMortisANS_RotateFromCurve::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (LastCurveValues.Find(MeshComp))
	{
		LastCurveValues.Remove(MeshComp);
	}
}
