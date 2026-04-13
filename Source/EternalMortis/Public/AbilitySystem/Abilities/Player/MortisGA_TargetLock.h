// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/MortisPlayerGameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "MortisGA_TargetLock.generated.h"

class AMortisPlayerCharacter;
class UGameplayEffect;
class UUserWidget;
struct FGameplayEventData;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API UMortisGA_TargetLock : public UMortisPlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UMortisGA_TargetLock();

	UFUNCTION(BlueprintCallable, Category = "Mortis|TargetLock")
	bool TryLockOnTarget();

	UFUNCTION(BlueprintCallable, Category = "Mortis|TargetLock")
	void OnTargetLockTick(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Mortis|TargetLock")
	void SwitchTarget(const FGameplayTag& InSwitchDirectionTag);

	UFUNCTION(BlueprintCallable, Category = "Mortis|TargetLock")
	void InitializeTargetLockState();

	UFUNCTION(BlueprintCallable, Category = "Mortis|TargetLock")
	void CleanupTargetLockState();

	UFUNCTION(BlueprintCallable, Category = "Mortis|TargetLock")
	void CancelTargetLockAbility();

	UFUNCTION(BlueprintPure, Category = "Mortis|TargetLock")
	AActor* GetCurrentLockedActor() const { return CurrentLockedActor; }

	UFUNCTION(BlueprintPure, Category = "Mortis|TargetLock")
	bool HasLockedTarget() const { return IsValid(CurrentLockedActor); }

protected:
	void GetAvailableActorsToLock(TArray<AActor*>& OutActors) const;
	AActor* GetBestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors) const;

	void GetAvailableActorsOnScreenSides(const TArray<AActor*>& InAvailableActors, TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight) const;

	bool IsSelectableLockOnTarget(AActor* InActor) const;
	bool CanMaintainLockOnTarget(AActor* InActor) const;
	bool HasGameplayTagOnActor(const AActor* InActor, const FGameplayTag& InTag) const;
	bool HasLineOfSightToTarget(AActor* InActor) const;
	float GetTargetScore(AActor* InActor) const;

	void DrawTargetLockWidget();
	void SetTargetLockWidgetPosition();

	void InitializeLockOnMovement();
	void ResetLockOnMovement();

	void ApplyLockOnMoveSpeedEffect();
	void RemoveLockOnMoveSpeedEffect();

	AMortisPlayerCharacter* GetMortisPlayerCharacterFromActorInfo() const;
	APlayerController* GetPlayerControllerFromActorInfo() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Trace")
	float BoxTraceDistance = 2500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Trace")
	FVector TraceBoxSize = FVector(2000.f, 2000.f, 600.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> BoxTraceObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Trace")
	TEnumAsByte<ECollisionChannel> LineOfSightTraceChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Trace")
	bool bShouldPersistentDebugShape = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Targeting")
	float MinTargetViewDot = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Targeting")
	float BreakLockDistance = 3500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Targeting")
	float TargetLockRotationInterpSpeed = 8.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Targeting")
	float TargetLockCameraPitchOffset = 8.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Widget")
	TSubclassOf<UUserWidget> TargetLockWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Widget")
	FVector TargetLockWidgetWorldOffset = FVector(0.f, 0.f, 100.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Movement")
	TSubclassOf<UGameplayEffect> LockOnMoveSpeedEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Tags")
	FGameplayTag DeadStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Tags")
	FGameplayTag RollingStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Tags")
	FGameplayTag BlockingStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Tags")
	FGameplayTag SwitchLeftDirectionTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Tags")
	FGameplayTag SwitchRightDirectionTag;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "TargetLock")
	TArray<TObjectPtr<AActor>> AvailableActorsToLock;

	UPROPERTY(BlueprintReadOnly, Category = "TargetLock")
	TObjectPtr<AActor> CurrentLockedActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "TargetLock")
	TObjectPtr<UUserWidget> DrawnTargetLockWidget = nullptr;

	FActiveGameplayEffectHandle LockOnMoveSpeedEffectHandle;

	bool bCachedOrientRotationToMovement = true;
	bool bCachedUseControllerRotationYaw = false;
};
