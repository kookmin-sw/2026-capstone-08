// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MortisPlayerGameplayAbility.h"
#include "Character/Player/MortisPlayerCharacter.h"
#include "Controllers/MortisPlayerController.h"
#include "Components/Combat/MortisPlayerCombatComponent.h"


AMortisPlayerCharacter* UMortisPlayerGameplayAbility::GetMortisPlayerCharacterFromActorInfo()
{
	if (!CachedMortisPlayerCharacter.IsValid())
		CachedMortisPlayerCharacter = Cast<AMortisPlayerCharacter>(CurrentActorInfo->AvatarActor);
	return CachedMortisPlayerCharacter.IsValid() ? CachedMortisPlayerCharacter.Get() : nullptr;
}

AMortisPlayerController* UMortisPlayerGameplayAbility::GetMortisPlayerControllerFromActorInfo()
{
	if (!CachedMortisPlayerController.IsValid())
		CachedMortisPlayerController = Cast<AMortisPlayerController>(CurrentActorInfo->PlayerController);
	return CachedMortisPlayerController.IsValid() ? CachedMortisPlayerController.Get() : nullptr;
}

UMortisPlayerCombatComponent* UMortisPlayerGameplayAbility::GetMortisPlayerCombatComponent()
{
	return GetMortisPlayerCharacterFromActorInfo()->GetMortisPlayerCombatComponent();
}
