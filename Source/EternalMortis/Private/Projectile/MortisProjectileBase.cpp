// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/MortisProjectileBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

AMortisProjectileBase::AMortisProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);

	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = InitialProjectileSpeed;
	ProjectileMovementComp->MaxSpeed = MaxProjectileSpeed;
	ProjectileMovementComp->ProjectileGravityScale = 0.f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bAutoActivate = false;

	InitialLifeSpan = LifeTime;
}

void AMortisProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovementComp->InitialSpeed = InitialProjectileSpeed;
	ProjectileMovementComp->MaxSpeed = MaxProjectileSpeed;

	InitialLifeSpan = LifeTime;

	if (ProjectileDamagePolicy == EMortisProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
	else
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}

	if (bLaunchOnBeginPlay)
	{
		LaunchProjectile();
	}
}

void AMortisProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ProjectileDamagePolicy != EMortisProjectileDamagePolicy::OnHit)
		return;

	HandleProjectileImpact(OtherActor, Hit);
}

void AMortisProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ProjectileDamagePolicy != EMortisProjectileDamagePolicy::OnBeginOverlap)
		return;

	if (OverlappedActors.Contains(OtherActor))
		return;

	OverlappedActors.AddUnique(OtherActor);

	HandleProjectileImpact(OtherActor, SweepResult);
}

bool AMortisProjectileBase::BP_HandleProjectileImpact_Implementation(APawn* HitPawn, const FHitResult& HitResult, const FGameplayEventData& Payload)
{
	return false;
}

void AMortisProjectileBase::HandleProjectileImpact(AActor* OtherActor, const FHitResult& HitResult)
{
	if (!OtherActor || OtherActor == this)
		return;

	if (bIgnoreInstigator && OtherActor == GetInstigator())
		return;

	if (bHasImpacted)
		return;

	BP_OnSpawnProjectileHitFX(HitResult.ImpactPoint);

	APawn* HitPawn = Cast<APawn>(OtherActor);

	// 월드나 벽에 맞은 경우
	if (!HitPawn)
	{
		if (bDestroyOnImpact)
			Destroy();

		return;
	}

	FGameplayEventData Payload;
	Payload.Instigator = GetInstigator();
	Payload.Target = HitPawn;
	Payload.OptionalObject = this;

	const bool bShouldDestroy = BP_HandleProjectileImpact(HitPawn, HitResult, Payload);

	if (bShouldDestroy && bDestroyOnImpact)
		PrepareDestroyAfterImpact();
}

void AMortisProjectileBase::PrepareDestroyAfterImpact()
{
	if (bHasImpacted)
		return;

	bHasImpacted = true;

	if (ProjectileCollisionBox)
		ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->StopMovementImmediately();
		ProjectileMovementComp->Deactivate();
	}

	if (DestroyDelayAfterImpact > 0.f)
		SetLifeSpan(DestroyDelayAfterImpact);
	else
		Destroy();
}

void AMortisProjectileBase::LaunchProjectile()
{
	LaunchProjectileInDirection(GetActorForwardVector());
}

void AMortisProjectileBase::LaunchProjectileInDirection(const FVector& InDirection)
{
	if (bHasLaunched)
		return;

	if (!ProjectileMovementComp)
		return;

	FVector LaunchDirection = InDirection;

	if (!LaunchDirection.Normalize())
	{
		LaunchDirection = DefaultLaunchDirection.GetSafeNormal();
	}

	if (LaunchDirection.IsNearlyZero())
	{
		LaunchDirection = GetActorForwardVector();
	}

	ProjectileMovementComp->Velocity = LaunchDirection * InitialProjectileSpeed;
	ProjectileMovementComp->Activate(true);

	bHasLaunched = true;
}

void AMortisProjectileBase::SetProjectileDamageEffectSpecHandle(FGameplayEffectSpecHandle InSpecHandle)
{
	ProjectileDamageEffectSpecHandle = InSpecHandle;
}
