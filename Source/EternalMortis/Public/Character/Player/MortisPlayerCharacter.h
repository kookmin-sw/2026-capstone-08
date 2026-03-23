// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MortisCharacterBase.h"
#include "GameplayTagContainer.h"
#include "MortisPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UMortisInputConfig;
class UMortisPlayerCombatComponent;
class UMortisPlayerUIComponent;
class UMortisEquipmentComponent;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisPlayerCharacter : public AMortisCharacterBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Camera")
	float MinZoomLength = 150.f;
	UPROPERTY(EditAnywhere, Category = "Camera")
	float MaxZoomLength = 450.f;
	UPROPERTY(EditAnywhere, Category = "Camera")
	float ZoomStep = 50.f;
	UPROPERTY(EditAnywhere, Category = "Camera")
	float ZoomInterpSpeed = 10.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float TargetZoomLength = 200.0f;

	AMortisPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	// IMortisCombatInterface Override
	virtual UMortisCombatComponent* GetCombatComponent() const override;

	// Attack Recovery Animations
	UFUNCTION(BlueprintCallable, Category = "Mortis|AttackRecovery")
	void SetRecoveryMontage(UAnimMontage* InMontage);

	UFUNCTION(BlueprintCallable, Category = "Mortis|AttackRecovery")
	void StopRecoveryMontage(float BlendOutTime = 0.1f);

	UFUNCTION(BlueprintCallable, Category = "Mortis|InputBuffer")
	void ExecuteBufferedAbility();

	UFUNCTION(BlueprintCallable, Category = "Mortis|InputBuffer")
	inline void SetBufferEnabled(bool bEnabled) { bCanBufferInput = bEnabled; }

protected:
	// APawn Override
	virtual void PossessedBy(AController* NewController) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UMortisPlayerCombatComponent* MortisPlayerCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UMortisPlayerUIComponent* PlayerUIComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	UMortisEquipmentComponent* EquipmentComponent;

	// Inputs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	UMortisInputConfig* InputConfigDataAsset;
	UPROPERTY(VisibleAnywhere, Category = "InputBuffer")
	bool bCanBufferInput = false;
	UPROPERTY(VisibleAnywhere, Category = "InputBuffer")
	FGameplayTag BufferedInputTag;

	// Attack Recovery Animations
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentRecoveryMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Mortis|AttackRecovery", meta = (AllowPrivateAccess = "true"))
	float RecoveryBlendOutTime = 0.1f;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_Zoom(const FInputActionValue& InputActionValue);

	void Input_AbilityInputPressed(FGameplayTag InputTag);
	void Input_AbilityInputReleased(FGameplayTag InputTag);

	bool IsBufferableAbility(FGameplayTag AbilityTag);

public:
	FORCEINLINE UMortisPlayerCombatComponent* GetMortisPlayerCombatComponent() const { return MortisPlayerCombatComponent; }
	FORCEINLINE UMortisPlayerUIComponent* GetPlayerUIComponent() const { return PlayerUIComponent; }
	

};
