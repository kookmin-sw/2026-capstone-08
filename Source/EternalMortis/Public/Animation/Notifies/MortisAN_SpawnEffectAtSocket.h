// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Types/MortisEnumTypes.h"
#include "MortisAN_SpawnEffectAtSocket.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisAN_SpawnEffectAtSocket : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Mortis")
	EMortisMeshSource MeshSource;
	
	UPROPERTY(EditAnywhere, Category = "Mortis")
	FName SocketName;

	UPROPERTY(EditAnywhere, Category = "Mortis")
	TObjectPtr<UNiagaraSystem> NiagaraEffect;

	UPROPERTY(EditAnywhere, Category = "Mortis")
	TObjectPtr<UParticleSystem> CascadeEffect;

	UPROPERTY(EditAnywhere, Category = "Mortis")
	bool bAttachToSocket = false;
};
