// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MortisBossEncounterController.generated.h"

class AMortisEnemyCharacter;
class APlayerController;
class UMortisEnemyUIComponent;
class UMortisWidgetBase;

UCLASS(Blueprintable)
class ETERNALMORTIS_API AMortisBossEncounterController : public AActor
{
	GENERATED_BODY()

public:
	AMortisBossEncounterController();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Mortis|BossEncounter")
	bool StartBossEncounter(APlayerController* OwningPlayer = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Mortis|BossEncounter")
	void FinishBossEncounter();

	UFUNCTION(BlueprintCallable, Category = "Mortis|BossEncounter")
	void RemoveBossHealthBar();

	UFUNCTION(BlueprintCallable, Category = "Mortis|BossEncounter")
	void SetBossActor(AMortisEnemyCharacter* NewBossActor);

	UFUNCTION(BlueprintPure, Category = "Mortis|BossEncounter")
	bool IsBossEncounterActive() const { return bEncounterStarted && !bEncounterFinished; }

	UFUNCTION(BlueprintPure, Category = "Mortis|BossEncounter")
	bool HasBossEncounterFinished() const { return bEncounterFinished; }

	UFUNCTION(BlueprintPure, Category = "Mortis|BossEncounter")
	AMortisEnemyCharacter* GetBossActor() const { return BossActor; }

	UFUNCTION(BlueprintPure, Category = "Mortis|BossEncounter")
	UMortisWidgetBase* GetActiveBossHealthBarWidget() const { return ActiveBossHealthBarWidget; }

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mortis|BossEncounter")
	TObjectPtr<AMortisEnemyCharacter> BossActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|BossEncounter|UI")
	TSubclassOf<UMortisWidgetBase> BossHealthBarWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|BossEncounter|UI")
	int32 BossHealthBarZOrder = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|BossEncounter|UI")
	bool bBroadcastInitialHealthSnapshot = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mortis|BossEncounter")
	bool bFinishEncounterOnBossDeath = true;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Mortis|BossEncounter")
	TObjectPtr<UMortisWidgetBase> ActiveBossHealthBarWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|BossEncounter")
	bool bEncounterStarted = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mortis|BossEncounter")
	bool bEncounterFinished = false;

	UFUNCTION(BlueprintImplementableEvent, Category = "Mortis|BossEncounter", meta = (DisplayName = "On Boss Encounter Started"))
	void BP_OnBossEncounterStarted(AMortisEnemyCharacter* InBossActor, UMortisWidgetBase* InBossHealthBarWidget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Mortis|BossEncounter", meta = (DisplayName = "On Boss Encounter Finished"))
	void BP_OnBossEncounterFinished(AMortisEnemyCharacter* InBossActor);

private:
	UFUNCTION()
	void HandleBossHealthChanged(float NewHealth, float MaxHealth);

	UMortisEnemyUIComponent* GetBossUIComponent() const;
	APlayerController* ResolveOwningPlayer(APlayerController* PreferredOwningPlayer) const;
	void BindBossHealthEvents(UMortisEnemyUIComponent* BossUIComponent);
	void UnbindBossHealthEvents();
	void BroadcastInitialHealthSnapshot(UMortisEnemyUIComponent* BossUIComponent);
};
