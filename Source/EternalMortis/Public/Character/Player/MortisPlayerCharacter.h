// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MortisCharacterBase.h"
#include "MortisPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UMortisInputConfig;
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

protected:
	// APawn Override
	virtual void PossessedBy(AController* NewController) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	UMortisInputConfig* InputConfigDataAsset;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
};
