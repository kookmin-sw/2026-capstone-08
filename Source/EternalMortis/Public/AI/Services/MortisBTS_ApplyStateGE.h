// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "BehaviorTree/BTService.h"
#include "MortisBTS_ApplyStateGE.generated.h"

class UGameplayEffect;

struct FBTApplyStateGEMemory
{
	FActiveGameplayEffectHandle ActiveHandle;
};
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBTS_ApplyStateGE : public UBTService
{
	GENERATED_BODY()

public:
	UMortisBTS_ApplyStateGE();

protected:
	//~ Begin UBTNode Interface
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface
	
	//~ Begin UBTAuxiliaryNode Interface
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//~ End UBTAuxiliaryNode Interface

	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<UGameplayEffect> StateGEClass;
};
