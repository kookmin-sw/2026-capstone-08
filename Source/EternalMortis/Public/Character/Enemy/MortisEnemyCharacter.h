// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/MortisCharacterBase.h"
#include "Types/MortisStructTypes.h"
#include "MortisEnemyCharacter.generated.h"

class UMortisEnemyCombatComponent;
class UMortisEnemyData;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisEnemyCharacter : public AMortisCharacterBase
{
	GENERATED_BODY()

public:
	AMortisEnemyCharacter(const FObjectInitializer& ObjectInitializer);

	void InitializeEnemyCharacter();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	//~ Begin APawn Interface
	// virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

public:
	UMortisEnemyData* GetEnemyData() const;

	//~ Begin IMortisCombatInterface
	FORCEINLINE virtual UMortisCombatComponent* GetCombatComponent() const override;
	//~ End IMortisCombatInterfac
	FORCEINLINE UMortisEnemyCombatComponent* GetEnemyCombatComponent() const;

	float GetRandomStrafingDistance() const;
protected:
	void InitializeEnemyByData();
	
	UPROPERTY(EditDefaultsOnly, Category = "Mortis|Data")
	TObjectPtr<UMortisEnemyData> EnemyData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|Component")
	TObjectPtr<UMortisEnemyCombatComponent> EnemyCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|Component")
	FGameplayTag CurrentPhase = MortisGameplayTags::State_Enemy_Phase_1;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	void RegisterStateTagEvent();
	void OnStrafingStateChanged(FGameplayTag Tag, int32 NewCount);
	void OnChasingStateChanged(FGameplayTag Tag, int32 NewCount) const;
};
