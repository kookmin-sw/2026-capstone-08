// Fill out your copyright notice in the Description page of Project Settings.


#include "MortisFunctionLibrary.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Controllers/MortisAIController.h"
#include "Interfaces/MortisCombatInterface.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"

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

float UMortisFunctionLibrary::GetGradeCoef(EMortisStatGrade Grade)
{
	switch (Grade)
	{
	case EMortisStatGrade::S: return 1.5f;
	case EMortisStatGrade::A: return 1.25f;
	case EMortisStatGrade::B: return 1.0f;
	case EMortisStatGrade::C: return 0.75f;
	case EMortisStatGrade::D: return 0.5f;
	case EMortisStatGrade::F: return 0.0f;
	default: return 0.0f;
	}
}

FString UMortisFunctionLibrary::GetGradeText(EMortisStatGrade Grade)
{
	switch (Grade)
	{
	case EMortisStatGrade::S: return TEXT("S");
	case EMortisStatGrade::A: return TEXT("A");
	case EMortisStatGrade::B: return TEXT("B");
	case EMortisStatGrade::C: return TEXT("C");
	case EMortisStatGrade::D: return TEXT("D");
	case EMortisStatGrade::F: return TEXT("F");
	default: return TEXT("F");
	}
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

AMortisEnemyCharacter* UMortisFunctionLibrary::GetEnemyCharacter(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	return AIC ? Cast<AMortisEnemyCharacter>(AIC->GetPawn()) : nullptr;
}

AMortisAIController* UMortisFunctionLibrary::GetMortisAIController(UBehaviorTreeComponent& OwnerComp)
{
	return Cast<AMortisAIController>(OwnerComp.GetAIOwner());
}

UMortisAbilitySystemComponent* UMortisFunctionLibrary::GetMortisASC(UBehaviorTreeComponent& OwnerComp)
{
	AMortisEnemyCharacter* Enemy = GetEnemyCharacter(OwnerComp);
	return Enemy ? Enemy->GetMortisAbilitySystemComponent() : nullptr;
}