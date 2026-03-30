// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/MortisEnumTypes.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "MortisFunctionLibrary.generated.h"

class UMortisAbilitySystemComponent;
class AMortisAIController;
class AMortisEnemyCharacter;
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

	UFUNCTION(BlueprintPure, Category = "Mortis|FunctionLibrary")
	static float GetGradeCoef(EMortisStatGrade Grade);

	UFUNCTION(BlueprintPure, Category = "Mortis|FunctionLibrary")
	static FString GetGradeText(EMortisStatGrade Grade);

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

	/* AI */
	static AMortisEnemyCharacter* GetEnemyCharacter(UBehaviorTreeComponent& OwnerComp);
	static AMortisAIController* GetMortisAIController(UBehaviorTreeComponent& OwnerComp);
	static UMortisAbilitySystemComponent* GetMortisASC(UBehaviorTreeComponent& OwnerComp);

	/* Motion Warping */
	static FVector CalculateWarpTargetLocation(const AActor* SelfActor, const AActor* TargetActor, EMortisWarpTargetMode WarpTargetMode, float DesiredDistance);
};
