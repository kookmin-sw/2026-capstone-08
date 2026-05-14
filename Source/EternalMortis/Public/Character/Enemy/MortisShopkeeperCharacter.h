// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Types/MortisRuneDataTypes.h"
#include "Types/MortisWeaponDataTypes.h"
#include "MortisShopkeeperCharacter.generated.h"

class AMortisWeaponShopItem;
class AMortisRuneShopItem;
class UMortisRunStateSubsystem;
class UMortisShopPricingSubsystem;

UCLASS()
class ETERNALMORTIS_API AMortisShopkeeperCharacter : public AMortisEnemyCharacter
{
	GENERATED_BODY()

public:
	AMortisShopkeeperCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	// 슬롯 등록
	UFUNCTION(BlueprintCallable)
	void RegisterWeaponItem(AMortisWeaponShopItem* ShopItem);

	UFUNCTION(BlueprintCallable)
	void UnregisterWeaponItem(AMortisWeaponShopItem* ShopItem);

	UFUNCTION(BlueprintCallable)
	void RegisterRuneItem(AMortisRuneShopItem* ShopItem);

	UFUNCTION(BlueprintCallable)
	void UnregisterRuneItem(AMortisRuneShopItem* ShopItem);

	// 공통 거래 체크
	UFUNCTION(BlueprintCallable)
	bool CanPurchase(APawn* InteractingPawn, int32 Price, bool bAlreadySold) const;

	UFUNCTION(BlueprintCallable)
	bool CanSteal(APawn* InteractingPawn, bool bAlreadySold, bool bCanSteal) const;

	UFUNCTION(BlueprintCallable)
	bool CommitPurchase(APawn* InteractingPawn, int32 Price);

	UFUNCTION(BlueprintCallable)
	bool CommitSteal(APawn* InteractingPawn);

	UFUNCTION(BlueprintCallable)
	void TriggerAggro(APawn* AggroTarget);

	UFUNCTION(BlueprintPure)
	bool IsAggroed() const { return bAggroed; }

	UFUNCTION(BlueprintCallable)
	void GenerateShopStock();

	UFUNCTION(BlueprintCallable, Category = "Shop|Pricing")
	bool GenerateWeaponShopItemPrice(const FMortisWeaponRow& WeaponData, int32& OutPrice) const;

	UFUNCTION(BlueprintCallable, Category = "Shop|Pricing")
	bool GenerateRuneShopItemPrice(const FMortisRuneInstance& RuneInstance, int32& OutPrice) const;

	UFUNCTION(BlueprintCallable, Category = "Shop|Stock")
	bool InitializeWeaponShopItemWithGeneratedPrice(AMortisWeaponShopItem* ShopItem, const FMortisWeaponRow& WeaponData);

	UFUNCTION(BlueprintCallable, Category = "Shop|Stock")
	bool InitializeRuneShopItemWithGeneratedPrice(AMortisRuneShopItem* ShopItem, const FMortisRuneInstance& RuneInstance);

protected:
	void SetAggroState(bool bNewAggroed, APawn* AggroTarget);
	UMortisRunStateSubsystem* GetRunStateSubsystem() const;
	UMortisShopPricingSubsystem* GetShopPricingSubsystem() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Shop|Stock")
	void BP_GenerateShopStock();
	virtual void BP_GenerateShopStock_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Shop|AI")
	void BP_OnAggroTriggered(APawn* AggroTarget);
	virtual void BP_OnAggroTriggered_Implementation(APawn* AggroTarget);

protected:
	// 레벨에 직접 꽂아서 관리
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Shop|Slots")
	TArray<TObjectPtr<AMortisWeaponShopItem>> WeaponSlots;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Shop|Slots")
	TArray<TObjectPtr<AMortisRuneShopItem>> RuneSlots;

	// 등록 확인용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop|Slots")
	TArray<TObjectPtr<AMortisWeaponShopItem>> RegisteredWeaponItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop|Slots")
	TArray<TObjectPtr<AMortisRuneShopItem>> RegisteredRuneItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop|AI")
	bool bAggroed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|AI")
	FName BlackboardAggroKey = TEXT("bAggroed");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|AI")
	FName BlackboardTargetActorKey = TEXT("TargetActor");
};
