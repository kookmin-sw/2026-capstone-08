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
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisPlayerCharacter : public AMortisCharacterBase
{
	GENERATED_BODY()
	
public:
	AMortisPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	// Attack Recovery Animations
	UFUNCTION(BlueprintCallable, Category = "Mortis|AttackRecovery")
	void SetRecoveryMontage(UAnimMontage* InMontage);

	UFUNCTION(BlueprintCallable, Category = "Mortis|AttackRecovery")
	void StopRecoveryMontage(float BlendOutTime = 0.1f);

protected:
	// APawn Override
	virtual void PossessedBy(AController* NewController) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

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
	// Inputs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	UMortisInputConfig* InputConfigDataAsset;
	// Attack Recovery Animations
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentRecoveryMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Mortis|AttackRecovery", meta = (AllowPrivateAccess = "true"))
	float RecoveryBlendOutTime = 0.1f;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);

	void Input_AbilityInputPressed(FGameplayTag InputTag);
	void Input_AbilityInputReleased(FGameplayTag InputTag);

public:
	FORCEINLINE UMortisPlayerCombatComponent* GetMortisPlayerCombatComponent() const { return MortisPlayerCombatComponent; }
};
