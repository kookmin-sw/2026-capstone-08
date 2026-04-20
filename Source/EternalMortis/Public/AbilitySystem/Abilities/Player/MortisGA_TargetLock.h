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

	// BP나 외부 로직에서 직접 쓸 가능성이 있는 것들은 public + UFUNCTION 유지
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

	AMortisPlayerCharacter* GetMortisPlayerCharacterFromActorInfo() const;
	APlayerController* GetPlayerControllerFromActorInfo() const;

protected:
	void GetAvailableActorsToLock(TArray<AActor*>& OutActors) const;
	AActor* GetBestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors) const;
	void GetAvailableActorsOnScreenSides(
		const TArray<AActor*>& InAvailableActors,
		TArray<AActor*>& OutActorsOnLeft,
		TArray<AActor*>& OutActorsOnRight) const;

	bool IsSelectableLockOnTarget(AActor* InActor) const;
	bool CanMaintainLockOnTarget(AActor* InActor) const;

	bool HasGameplayTagOnActor(const AActor* InActor, const FGameplayTag& InTag) const;
	bool HasAnyMatchingTagOnActor(const AActor* InActor, const FGameplayTagContainer& InTags) const;

	bool ShouldRotateCharacterToTarget(const AMortisPlayerCharacter* PlayerCharacter) const;
	bool ShouldOrientRotationToMovement(const AMortisPlayerCharacter* PlayerCharacter) const;

	bool HasLineOfSightToTarget(AActor* InActor) const;
	float GetTargetScore(AActor* InActor) const;

	void DrawTargetLockWidget();
	void SetTargetLockWidgetPosition();

	void InitializeLockOnMovement();
	void ResetLockOnMovement();

	void ApplyLockOnMoveSpeedEffect();
	void RemoveLockOnMoveSpeedEffect();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|StateTags")
	FGameplayTag DeadStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|StateTags")
	FGameplayTagContainer CameraOnlyLockTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|StateTags")
	FGameplayTagContainer OrientToMovementTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|StateTags")
	FGameplayTagContainer ForceCharacterRotateToTargetTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|StateTags")
	FGameplayTag SwitchLeftDirectionTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|StateTags")
	FGameplayTag SwitchRightDirectionTag;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "TargetLock")
	TArray<AActor*> AvailableActorsToLock;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "TargetLock")
	AActor* CurrentLockedActor = nullptr;

	UPROPERTY()
	UUserWidget* DrawnTargetLockWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock")
	float CharacterRotationInterpSpeed = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock")
	float CharacterRotationRecoverInterpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock")
	float BreakLockDistance = 1200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock")
	float TargetLockRotationInterpSpeed = 12.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock")
	float TargetLockCameraPitchOffset = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock")
	float BoxTraceDistance = 1200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock")
	FVector TraceBoxSize = FVector(500.f, 500.f, 300.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock")
	TArray<TEnumAsByte<EObjectTypeQuery>> BoxTraceObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock")
	TEnumAsByte<ECollisionChannel> LineOfSightTraceChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock")
	float MinTargetViewDot = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock")
	bool bShouldPersistentDebugShape = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|UI")
	TSubclassOf<UUserWidget> TargetLockWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|UI")
	FVector TargetLockWidgetWorldOffset = FVector(0.f, 0.f, 100.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetLock|Effect")
	TSubclassOf<UGameplayEffect> LockOnMoveSpeedEffectClass;

	FActiveGameplayEffectHandle LockOnMoveSpeedEffectHandle;
};