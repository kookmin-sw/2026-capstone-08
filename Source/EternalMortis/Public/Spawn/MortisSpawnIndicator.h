#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "Types/MortisEnumTypes.h"
#include "Types/MortisStructTypes.h"
#include "MortisSpawnIndicator.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
struct FGameplayEffectSpecHandle;

UCLASS()
class ETERNALMORTIS_API AMortisSpawnIndicator : public AActor
{
	GENERATED_BODY()
	
public:
	AMortisSpawnIndicator();
	
	void InitAndStart(TSubclassOf<AActor> ActorClass, const FGameplayEffectSpecHandle& DamageSpecHandle, APawn* InstigatorPawn);
	
	FORCEINLINE void SetClassToSpawn(TSubclassOf<AActor> NewClassToSpawn) { ClassToSpawn = NewClassToSpawn; }
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Mortis|Indicator")
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(VisibleAnywhere, Category = "Mortis|Indicator")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mortis|Indicator")
	TObjectPtr<UParticleSystemComponent> CascadeComponent;
	
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0"), Category = "Mortis|Indicator")
	float Delay;
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetTimeScale();
 
	UFUNCTION()
	void OnNiagaraSystemFinished(UNiagaraComponent* NiagaraComp);
	
	UFUNCTION()
	void OnParticleSystemFinished(UParticleSystemComponent* ParticleSystemComp);
	
private:
	TSubclassOf<AActor> ClassToSpawn;
	FGameplayEffectSpecHandle CachedDamageSpecHandle;
	TWeakObjectPtr<APawn> CachedInstigator;
	
	FTimerHandle SpawnTimerHandle;
	
	void OnDelayFinished();
};
