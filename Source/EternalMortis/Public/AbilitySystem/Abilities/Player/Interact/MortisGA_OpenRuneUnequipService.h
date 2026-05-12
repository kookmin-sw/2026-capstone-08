#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/MortisPlayerGameplayAbility.h"
#include "MortisGA_OpenRuneUnequipService.generated.h"

UCLASS(Blueprintable)
class ETERNALMORTIS_API UMortisGA_OpenRuneUnequipService : public UMortisPlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UMortisGA_OpenRuneUnequipService();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};
