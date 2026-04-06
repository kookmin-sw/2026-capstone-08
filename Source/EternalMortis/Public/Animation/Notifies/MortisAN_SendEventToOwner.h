// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MortisAN_SendEventToOwner.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Mortis Send Gameplay Event To Owner"))
class ETERNALMORTIS_API UMortisAN_SendEventToOwner : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Mortis")
	FGameplayTag EventTag;
};
