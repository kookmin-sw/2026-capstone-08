// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/MortisCharacterBase.h"
#include "Types/MortisStructTypes.h"
#include "MortisEnemyCharacter.generated.h"

class UMortisEnemyCombatComponent;
class UMortisEnemyData;
class UMortisEnemyUIComponent;
class UWidgetComponent;
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
	
	//~ Begin AActor Interfaces 
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	//~ End AActor Interfaces 
	
	//~ Begin AMortisCharacterBase Interfaces
	virtual void StartDeath() override;
	virtual void FinishDeath() override;
	//~ End AMortisCharacterBase Interfaces

	//~ Begin IMortisCombatInterface
	FORCEINLINE virtual UMortisCombatComponent* GetCombatComponent() const override;
	//~ End IMortisCombatInterface

public:
	UMortisEnemyData* GetEnemyData() const;

	void SetEnemyHealthBarCombatVisibility(bool bShouldShow);
	
	UMortisEnemyCombatComponent* GetEnemyCombatComponent() const;
	FORCEINLINE UMortisEnemyUIComponent* GetEnemyUIComponent() const { return EnemyUIComponent; }

	float GetRandomStrafingDistance() const;
	
protected:
	void InitializeEnemyByData();
	void InitializeEnemyHUD();
	void UpdateEnemyHealthBarWidgetLocation();
	void ApplyMaterialSet(const FMortisMaterialSet& MaterialSet);
	
	UPROPERTY(EditDefaultsOnly, Category = "Mortis|Data")
	TObjectPtr<UMortisEnemyData> EnemyData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|Component")
	TObjectPtr<UMortisEnemyCombatComponent> EnemyCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|Component")
	TObjectPtr<UMortisEnemyUIComponent> EnemyUIComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mortis|UI")
	TObjectPtr<UWidgetComponent> EnemyHealthBarWidgetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|UI")
	float EnemyHealthBarHeightOffset = 30.f;

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
