// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "MortisProjectileBase.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;

UENUM(BlueprintType)
enum class EMortisProjectileDamagePolicy : uint8
{
	OnHit,
	OnBeginOverlap
};


UCLASS()
class ETERNALMORTIS_API AMortisProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:
	AMortisProjectileBase();

	UFUNCTION(BlueprintCallable, Category = "Mortis|Projectile")
	void LaunchProjectile();

	UFUNCTION(BlueprintCallable, Category = "Mortis|Projectile")
	void LaunchProjectileInDirection(const FVector& InDirection);

	UFUNCTION(BlueprintCallable, Category = "Mortis|Projectile|Damage")
	void SetProjectileDamageEffectSpecHandle(FGameplayEffectSpecHandle InSpecHandle);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Projectile")
	float DestroyDelayAfterImpact = 0.f;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Projectile")
	TObjectPtr<UBoxComponent> ProjectileCollisionBox;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Projectile")
	TObjectPtr<UNiagaraComponent> ProjectileNiagaraComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Projectile")
	EMortisProjectileDamagePolicy ProjectileDamagePolicy = EMortisProjectileDamagePolicy::OnHit;

	// SpawnActor 할 때 넘길 수 있게
	UPROPERTY(BlueprintReadOnly, Category = "Mortis|Projectile|Damage", meta = (ExposeOnSpawn = "true"))
	FGameplayEffectSpecHandle ProjectileDamageEffectSpecHandle;

	// true면 BeginPlay에서 바로 발사
	// false면 LaunchProjectile() 호출 전까지 대기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Projectile|Launch")
	bool bLaunchOnBeginPlay = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Projectile|Launch")
	float InitialProjectileSpeed = 700.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Projectile|Launch")
	float MaxProjectileSpeed = 900.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Projectile|Launch")
	FVector DefaultLaunchDirection = FVector::ForwardVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Projectile")
	bool bDestroyOnImpact = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Projectile")
	bool bIgnoreInstigator = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Projectile")
	float LifeTime = 10.f;

protected:
	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 충돌 FX용
	UFUNCTION(BlueprintImplementableEvent, Category = "Mortis|Projectile", meta = (DisplayName = "On Spawn Projectile Hit FX"))
	void BP_OnSpawnProjectileHitFX(const FVector& HitLocation);

	// 여기서 BP가 데미지 적용, Gameplay Event 전송 처리
	// return true면 C++ 쪽에서 Destroy
	UFUNCTION(BlueprintNativeEvent, Category = "Mortis|Projectile", meta = (DisplayName = "Handle Projectile Impact"))
	bool BP_HandleProjectileImpact(APawn* HitPawn, const FHitResult& HitResult, const FGameplayEventData& Payload);
	virtual bool BP_HandleProjectileImpact_Implementation(APawn* HitPawn, const FHitResult& HitResult, const FGameplayEventData& Payload);

private:
	void HandleProjectileImpact(AActor* OtherActor, const FHitResult& HitResult);
	void PrepareDestroyAfterImpact();

private:
	UPROPERTY()
	TArray<TObjectPtr<AActor>> OverlappedActors;

	bool bHasLaunched = false;
	bool bHasImpacted = false;
};
