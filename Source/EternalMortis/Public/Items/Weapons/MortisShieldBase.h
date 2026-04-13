// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MortisWeaponBase.h"
#include "MortisShieldBase.generated.h"

DECLARE_DELEGATE_OneParam(FOnShieldBlockDelegate, AActor*)
/**
 * 
 */
UCLASS()
class ETERNALMORTIS_API AMortisShieldBase : public AMortisWeaponBase
{
	GENERATED_BODY()
	
public:
	FOnShieldBlockDelegate OnShieldBeginBlock;
	FOnShieldBlockDelegate OnShieldEndBlock;

protected:
	virtual void BeginPlay() override;
	//~Begin AMortisCombatItemBase Interfaces
	virtual void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	//~End AMortisCombatItemBase Interfaces
};
