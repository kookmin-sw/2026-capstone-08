// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MortisANS_ActivateInputBuffer.generated.h"

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisANS_ActivateInputBuffer : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	//~ Begin UAnimNotifyState Interface
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	//~ End UAnimNotifyState Interface

private:
	void ToggleInputBuffer(USkeletalMeshComponent* MeshComp, bool bEnable);
};
