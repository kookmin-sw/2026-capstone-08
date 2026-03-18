// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/MortisStructTypes.h"

#include "GameplayAbilitySpecHandle.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Delegates/IDelegateInstance.h"

#include "MortisBTT_ExecuteAttackPattern.generated.h"

struct FAbilityEndedData;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisBTT_ExecuteAttackPattern : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMortisBTT_ExecuteAttackPattern();
	
	//~ Begin UBTNode Interface 
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	//~ Begin UBTNode Interface
	
	//~ Begin UBTTaskNode Interface 
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	//~ Begin UBTTaskNode Interface	

protected:
	UPROPERTY(EditAnywhere)
	// FName TargetDistanceKey;
	FBlackboardKeySelector TargetDistanceKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetActorKey;

private:
	UPROPERTY()
	FGameplayAbilitySpecHandle ActivatedHandle;

	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	UPROPERTY()
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	
	FDelegateHandle EndDelegateHandle;

	UFUNCTION()
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
};
