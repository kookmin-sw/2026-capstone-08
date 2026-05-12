// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MortisEnemySpawnPointComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ETERNALMORTIS_API UMortisEnemySpawnPointComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMortisEnemySpawnPointComponent();

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, Category = "Mortis|Spawn")
	TObjectPtr<UBillboardComponent> SpriteComponent;
	
	virtual void OnRegister() override;
#endif
};
