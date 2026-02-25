// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/MortisEnumTypes.h"
#include "MortisFunctionLibrary.generated.h"

class UMortisCombatComponent;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Mortis|FunctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);

	/* Combat */
	static UMortisCombatComponent* GetCombatComponent(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Combat", meta = (DisplayName = "Get Combat Component", ExpandEnumAsExecs = "OutValidType"))
	static UMortisCombatComponent* BP_GetCombatComponent(const AActor* Actor, EMortisValidType& OutValidType);

	/* Gameplay tag */
	UFUNCTION(BlueprintCallable, Category = "Mortis|GameplayTag", meta = (DisplayName = "Has Gameplay Tag", ExpandBoolAsExecs = "ReturnValue"))
	static bool HasGameplayTag(AActor* Actor, const FGameplayTag& TagToCheck);

	UFUNCTION(BlueprintCallable, Category = "Mortis|GameplayTag")
	static void AddGameplayTag(AActor* Actor, const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, Category = "Mortis|GameplayTag")
	static void RemoveGameplayTag(AActor* Actor, const FGameplayTag& Tag);	
};
