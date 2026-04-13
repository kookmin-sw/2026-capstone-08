// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/MortisAT_ExecuteTaskOnTick.h"

UMortisAT_ExecuteTaskOnTick::UMortisAT_ExecuteTaskOnTick()
{
	bTickingTask = true;
}

UMortisAT_ExecuteTaskOnTick* UMortisAT_ExecuteTaskOnTick::ExecuteTaskOnTick(UGameplayAbility* OwningAbility)
{
	UMortisAT_ExecuteTaskOnTick* Node = NewAbilityTask<UMortisAT_ExecuteTaskOnTick>(OwningAbility);
	return Node;
}

void UMortisAT_ExecuteTaskOnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (ShouldBroadcastAbilityTaskDelegates())
		OnAbilityTaskTick.Broadcast(DeltaTime);
	else
		EndTask();
}
