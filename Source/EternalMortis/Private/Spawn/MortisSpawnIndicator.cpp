#include "Spawn/MortisSpawnIndicator.h"

#include "MortisDebugHelper.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Projectile/MortisProjectileBase.h"
#include "Spawn/MortisHitboxBase.h"

AMortisSpawnIndicator::AMortisSpawnIndicator()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(GetRootComponent());
	NiagaraComponent->bAutoActivate = false;
	
	CascadeComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("CascadeComponent"));
	CascadeComponent->SetupAttachment(GetRootComponent());
	CascadeComponent->bAutoActivate = false;
}

void AMortisSpawnIndicator::InitAndStart(TSubclassOf<AActor> ActorClass, const FGameplayEffectSpecHandle& DamageSpecHandle, APawn* InstigatorPawn)
{
	ClassToSpawn = ActorClass;
	CachedDamageSpecHandle = DamageSpecHandle;
	CachedInstigator = InstigatorPawn;
	
	if (NiagaraComponent && NiagaraComponent->GetAsset())
	{
		NiagaraComponent->ActivateSystem();
		NiagaraComponent->OnSystemFinished.AddDynamic(this, &ThisClass::OnNiagaraSystemFinished);
	}
	else if (CascadeComponent && CascadeComponent->Template)
	{
		CascadeComponent->SetWorldLocation(GetActorLocation());
		CascadeComponent->Activate(true);
		CascadeComponent->OnSystemFinished.AddDynamic(this, &ThisClass::OnParticleSystemFinished);
	}
	
	BP_SetTimeScale();
	
	if (Delay > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ThisClass::OnDelayFinished, Delay);
	}
	else
	{
		OnDelayFinished();
	}
}

void AMortisSpawnIndicator::OnNiagaraSystemFinished(UNiagaraComponent* NiagaraComp)
{
	Destroy();
}

void AMortisSpawnIndicator::OnParticleSystemFinished(UParticleSystemComponent* ParticleSystemComp)
{
	Destroy();
}

void AMortisSpawnIndicator::OnDelayFinished()
{
	if (!ClassToSpawn || !CachedInstigator.IsValid())
	{
		MORTIS_LOG("");
		return;
	}

	FActorSpawnParameters SpawnParams;	
	SpawnParams.Instigator = CachedInstigator.Get();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	if (!ClassToSpawn)
	{
		MORTIS_LOG("ClassToSpawn is invalid");
	}
	
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ClassToSpawn, GetActorLocation(), GetActorRotation(), SpawnParams);
	if (!Spawned)
	{
		MORTIS_LOG("Spawned actor is invalid");
		return;
	}
	if (AMortisProjectileBase* Projectile = Cast<AMortisProjectileBase>(Spawned))
	{
		Projectile->InitializeProjectile(CachedDamageSpecHandle);
	}
	else if (AMortisHitboxBase* Hitbox = Cast<AMortisHitboxBase>(Spawned))
	{
		Hitbox->InitializeHitbox(CachedDamageSpecHandle);
	}
}
