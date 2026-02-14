// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MortisCharacterBase.h"
#include "MortisEnemyCharacter.generated.h"

class UMortisEnemyData;
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisEnemyCharacter : public AMortisCharacterBase
{
	GENERATED_BODY()

public:
	AMortisEnemyCharacter();

protected:
	virtual void BeginPlay() override;

	//~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

	void InitializeEnemyByData();
	
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UMortisEnemyData> EnemyData;

public:
	UMortisEnemyData* GetEnemyData() const;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
