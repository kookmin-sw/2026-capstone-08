#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "Types/MortisEnumTypes.h"
#include "MortisHitboxBase.generated.h"

struct FGameplayEventData;
class UBoxComponent;
class UNiagaraComponent;
class UParticleSystemComponent;

UCLASS()
class ETERNALMORTIS_API AMortisHitboxBase : public AActor
{
    GENERATED_BODY()

public:
    AMortisHitboxBase();

    UFUNCTION(BlueprintCallable, Category = "Mortis|Hitbox")
    void InitializeHitbox(const FGameplayEffectSpecHandle& DamageSpecHandle);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Hitbox")
    float DestroyDelayAfterOverlap = 0.f;

protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Hitbox")
    TObjectPtr<UBoxComponent> HitboxCollisionBox;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Hitbox")
    TObjectPtr<UNiagaraComponent> HitboxNiagaraComponent;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Hitbox")
    TObjectPtr<UParticleSystemComponent> HitboxCascadeComponent;
    
    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Hitbox", meta = (ExposeOnSpawn = "true"))
    FGameplayEffectSpecHandle HitboxDamageEffectSpecHandle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Hitbox")
    bool bDestroyOnOverlap = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Hitbox")
    float LifeTime = 5.f;

protected:
    UFUNCTION()
    virtual void OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(BlueprintImplementableEvent, Category = "Mortis|Hitbox")
    void BP_OnHitboxOverlappedFX(const FVector& HitLocation);

    UFUNCTION(BlueprintNativeEvent, Category = "Mortis|Hitbox")
    bool BP_HandleHitboxOverlap(APawn* HitPawn, const FHitResult& HitResult, const FGameplayEventData& Payload);
    virtual bool BP_HandleHitboxOverlap_Implementation(APawn* HitPawn, const FHitResult& HitResult, const FGameplayEventData& Payload);

private:
    void PrepareDestroy();
    
    UPROPERTY()
    TArray<TObjectPtr<AActor>> HitActors;

    bool bHasInitialized = false;
};