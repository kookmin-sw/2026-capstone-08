// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "MortisTutorialPromptBoxComponent.generated.h"

class UUserWidget;

UCLASS(ClassGroup = (Mortis), meta = (BlueprintSpawnableComponent))
class ETERNALMORTIS_API UMortisTutorialPromptBoxComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	UMortisTutorialPromptBoxComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// Box에 들어왔을 때 Viewport에 띄울 WBP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Tutorial")
	TSubclassOf<UUserWidget> PromptWidgetClass;

	// Viewport 표시 우선순위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|Tutorial")
	int32 ZOrder = 10;

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> PromptWidgetInstance;

	int32 PlayerOverlapCount = 0;

private:
	UFUNCTION()
	void OnPromptBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPromptBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool IsPlayerActor(AActor* OtherActor) const;

	void ShowPromptWidget();
	void HidePromptWidget();
};
