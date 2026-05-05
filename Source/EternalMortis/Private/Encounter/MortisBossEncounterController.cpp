// Fill out your copyright notice in the Description page of Project Settings.

#include "Encounter/MortisBossEncounterController.h"

#include "Blueprint/UserWidget.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Components/UI/MortisEnemyUIComponent.h"
#include "Engine/World.h"
#include "UI/MortisWidgetBase.h"

AMortisBossEncounterController::AMortisBossEncounterController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMortisBossEncounterController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindBossHealthEvents();
	RemoveBossHealthBar();

	Super::EndPlay(EndPlayReason);
}

bool AMortisBossEncounterController::StartBossEncounter(APlayerController* OwningPlayer)
{
	if (bEncounterFinished || bEncounterStarted)
	{
		return IsBossEncounterActive();
	}

	UMortisEnemyUIComponent* BossUIComponent = GetBossUIComponent();
	APlayerController* ResolvedOwningPlayer = ResolveOwningPlayer(OwningPlayer);
	if (!BossActor || !BossUIComponent || !ResolvedOwningPlayer || !BossHealthBarWidgetClass)
	{
		return false;
	}

	ActiveBossHealthBarWidget = CreateWidget<UMortisWidgetBase>(ResolvedOwningPlayer, BossHealthBarWidgetClass);
	if (!ActiveBossHealthBarWidget)
	{
		return false;
	}

	ActiveBossHealthBarWidget->AddToViewport(BossHealthBarZOrder);
	ActiveBossHealthBarWidget->BP_BindUIComponent(BossUIComponent);

	BindBossHealthEvents(BossUIComponent);

	bEncounterStarted = true;
	bEncounterFinished = false;

	BP_OnBossEncounterStarted(BossActor, ActiveBossHealthBarWidget);
	BroadcastInitialHealthSnapshot(BossUIComponent);

	return true;
}

void AMortisBossEncounterController::FinishBossEncounter()
{
	if (bEncounterFinished)
	{
		return;
	}

	AMortisEnemyCharacter* FinishedBossActor = BossActor;

	bEncounterStarted = false;
	bEncounterFinished = true;

	UnbindBossHealthEvents();
	RemoveBossHealthBar();

	BP_OnBossEncounterFinished(FinishedBossActor);
}

void AMortisBossEncounterController::RemoveBossHealthBar()
{
	if (ActiveBossHealthBarWidget)
	{
		ActiveBossHealthBarWidget->RemoveFromParent();
		ActiveBossHealthBarWidget = nullptr;
	}
}

void AMortisBossEncounterController::SetBossActor(AMortisEnemyCharacter* NewBossActor)
{
	if (BossActor == NewBossActor)
	{
		return;
	}

	UnbindBossHealthEvents();
	BossActor = NewBossActor;
}

void AMortisBossEncounterController::HandleBossHealthChanged(float NewHealth, float MaxHealth)
{
	(void)MaxHealth;

	if (bFinishEncounterOnBossDeath && NewHealth <= 0.f)
	{
		FinishBossEncounter();
	}
}

UMortisEnemyUIComponent* AMortisBossEncounterController::GetBossUIComponent() const
{
	return BossActor ? BossActor->GetEnemyUIComponent() : nullptr;
}

APlayerController* AMortisBossEncounterController::ResolveOwningPlayer(APlayerController* PreferredOwningPlayer) const
{
	if (PreferredOwningPlayer)
	{
		return PreferredOwningPlayer;
	}

	UWorld* World = GetWorld();
	return World ? World->GetFirstPlayerController() : nullptr;
}

void AMortisBossEncounterController::BindBossHealthEvents(UMortisEnemyUIComponent* BossUIComponent)
{
	if (BossUIComponent)
	{
		BossUIComponent->OnHealthChanged.AddUniqueDynamic(this, &ThisClass::HandleBossHealthChanged);
	}
}

void AMortisBossEncounterController::UnbindBossHealthEvents()
{
	if (UMortisEnemyUIComponent* BossUIComponent = GetBossUIComponent())
	{
		BossUIComponent->OnHealthChanged.RemoveDynamic(this, &ThisClass::HandleBossHealthChanged);
	}
}

void AMortisBossEncounterController::BroadcastInitialHealthSnapshot(UMortisEnemyUIComponent* BossUIComponent)
{
	if (!bBroadcastInitialHealthSnapshot || !BossUIComponent)
	{
		return;
	}

	float CurrentHealth = 0.f;
	float MaxHealth = 0.f;
	if (BossUIComponent->GetCurrentHealthSnapshot(CurrentHealth, MaxHealth))
	{
		BossUIComponent->OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	}
}
