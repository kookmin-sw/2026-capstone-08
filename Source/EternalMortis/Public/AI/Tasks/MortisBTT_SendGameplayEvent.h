// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MortisBTT_SendGameplayEvent.generated.h"

class UAbilitySystemComponent;

struct FBTSendGameplayEventTaskMemory
{
	TWeakObjectPtr<AActor> CachedReceiver;
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	bool IsValid()
	{
		return CachedReceiver.IsValid() && CachedASC.IsValid();
	}

	void Reset()
	{
		CachedReceiver.Reset();
		CachedASC.Reset();
	}
};
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBTT_SendGameplayEvent : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMortisBTT_SendGameplayEvent();

	//~ Begin UBTNode Interface 
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual uint16 GetInstanceMemorySize() const override;
	//~ End UBTNode Interface 

	//~ Begin UBTTaskNode Interface
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	//~ End UBTTaskNode Interface

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag StateTagToWait; 

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector EventReceiverKey;
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PayloadTargetKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector EventMagnitude;
};
