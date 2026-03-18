// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/MortisCharacterBase.h"
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

protected:
	virtual void BeginPlay() override;

	//~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

public:
	UMortisEnemyData* GetEnemyData() const;

	//~ Begin IMortisCombatInterface
	FORCEINLINE virtual UMortisCombatComponent* GetCombatComponent() const override;
	//~ End IMortisCombatInterfac
	FORCEINLINE UMortisEnemyCombatComponent* GetEnemyCombatComponent() const;
	
protected:
	void InitializeEnemyByData();
	
	UPROPERTY(EditDefaultsOnly, Category = "Mortis|Data")
	TObjectPtr<UMortisEnemyData> EnemyData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|Component")
	TObjectPtr<UMortisEnemyCombatComponent> EnemyCombatComponent;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	void RegisterStateTagEvent();
	void OnStrafingStateChanged(FGameplayTag Tag, int32 NewCount);
	void OnChasingStateChanged(FGameplayTag Tag, int32 NewCount) const;

	float IdleMaxWalkSpeed;
	float StrafingMaxWalkSpeed;
	float ChasingMaxWalkSpeed;	
};
