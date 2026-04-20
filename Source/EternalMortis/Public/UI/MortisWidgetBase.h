// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MortisWidgetBase.generated.h"

class UMortisUIComponent;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Mortis|UI", meta = (DisplayName = "Bind UI Component"))
	void BP_BindUIComponent(UMortisUIComponent* InUIComponent);
};
