// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MortisANS_RotateFromCurve.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisANS_RotateFromCurve : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	//~ Begin UAnimNotifyState Interface
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	//~ End UAnimNotifyState Interface

protected:
	UPROPERTY(EditAnywhere)
	FName CurveName = TEXT("DistanceCurve");

private:
	TMap<TWeakObjectPtr<USkeletalMeshComponent>, float> LastCurveValues;
};
