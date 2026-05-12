#include "AbilitySystem/Abilities/Player/Interact/MortisGA_OpenRuneUnequipService.h"

#include "Character/Player/MortisPlayerCharacter.h"
#include "Components/Collisions/MortisInteractionComponent.h"
#include "Items/Interactable/MortisInteractableActorBase.h"
#include "Items/Interactable/Service/MortisRuneUnequipServiceActor.h"
#include "MortisDebugHelper.h"
#include "Types/MortisEnumTypes.h"

UMortisGA_OpenRuneUnequipService::UMortisGA_OpenRuneUnequipService()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityActivationPolicy = EMortisAbilityActivationPolicy::OnGiven;
}

void UMortisGA_OpenRuneUnequipService::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bool bSucceeded = false;

	AMortisPlayerCharacter* PlayerCharacter = GetMortisPlayerCharacterFromActorInfo();
	UMortisInteractionComponent* InteractionComponent = PlayerCharacter
		? PlayerCharacter->FindComponentByClass<UMortisInteractionComponent>()
		: nullptr;

	AMortisInteractableActorBase* Target = InteractionComponent ? InteractionComponent->GetPendingTarget() : nullptr;
	if (!Target && InteractionComponent)
	{
		Target = InteractionComponent->GetSelectedTarget();
	}

	AMortisRuneUnequipServiceActor* RuneUnequipServiceActor = Cast<AMortisRuneUnequipServiceActor>(Target);
	if (!PlayerCharacter || !RuneUnequipServiceActor)
	{
		MORTIS_LOG("Rune unequip service interaction failed: invalid player or target.");
	}
	else if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		MORTIS_LOG("Rune unequip service interaction failed: ability commit failed.");
	}
	else if (!RuneUnequipServiceActor->CanInteract(PlayerCharacter))
	{
		MORTIS_LOG("Rune unequip service interaction failed: service actor cannot interact.");
	}
	else
	{
		Target->OnInteractionFinished(PlayerCharacter, true);
		bSucceeded = true;
	}

	if (!bSucceeded && Target)
	{
		Target->OnInteractionFinished(PlayerCharacter, false);
	}

	if (InteractionComponent)
	{
		InteractionComponent->ClearPendingTarget();
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, !bSucceeded);
}
