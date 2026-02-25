// Fill out your copyright notice in the Description page of Project Settings.


#include "MortisFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Interfaces/MortisCombatInterface.h"

bool UMortisFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);

	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (QueryTeamAgent && TargetTeamAgent)
	{
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}

	return false;
}

UMortisCombatComponent* UMortisFunctionLibrary::GetCombatComponent(const AActor* Actor)
{
	if (const IMortisCombatInterface* CombatInterface = Cast<IMortisCombatInterface>(Actor))
	{
		return CombatInterface->GetCombatComponent();
	}
	return nullptr;
}

UMortisCombatComponent* UMortisFunctionLibrary::BP_GetCombatComponent(const AActor* Actor, EMortisValidType& OutValidType)
{
	UMortisCombatComponent* CombatComponent = GetCombatComponent(Actor); 
	OutValidType = CombatComponent ? EMortisValidType::Valid : EMortisValidType::InValid;
	return CombatComponent;
}

bool UMortisFunctionLibrary::HasGameplayTag(AActor* Actor, const FGameplayTag& TagToCheck)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
	{
		return ASC->HasMatchingGameplayTag(TagToCheck);
	}
	return false;
}

void UMortisFunctionLibrary::AddGameplayTag(AActor* Actor, const FGameplayTag& Tag)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	if (ASC && !ASC->HasMatchingGameplayTag(Tag))
	{
		ASC->AddLooseGameplayTag(Tag);
	}
}

void UMortisFunctionLibrary::RemoveGameplayTag(AActor* Actor, const FGameplayTag& Tag)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	if (ASC && ASC->HasMatchingGameplayTag(Tag))
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
}
