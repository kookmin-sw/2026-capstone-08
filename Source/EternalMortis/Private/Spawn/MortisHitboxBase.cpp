#include "Spawn/MortisHitboxBase.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Character/MortisCharacterBase.h"
#include "MortisDebugHelper.h"

#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Character/Player/MortisPlayerCharacter.h"
#include "Particles/ParticleSystemComponent.h"

AMortisHitboxBase::AMortisHitboxBase()
{
    PrimaryActorTick.bCanEverTick = false;

    HitboxCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitboxCollisionBox"));
    SetRootComponent(HitboxCollisionBox);
    HitboxCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    HitboxCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    HitboxCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    HitboxNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    HitboxNiagaraComponent->SetupAttachment(GetRootComponent());

    HitboxCascadeComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("CascadeComponent"));
    HitboxCascadeComponent->SetupAttachment(GetRootComponent());
}

void AMortisHitboxBase::BeginPlay()
{
    Super::BeginPlay();

    InitialLifeSpan = LifeTime;
    HitboxCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnHitboxBeginOverlap);

    if (HitboxDamageEffectSpecHandle.IsValid())
    {
        InitializeHitbox(HitboxDamageEffectSpecHandle);
    }
}

void AMortisHitboxBase::InitializeHitbox(const FGameplayEffectSpecHandle& DamageSpecHandle)
{
    if (bHasInitialized)
    {
        return;
    }

    HitboxDamageEffectSpecHandle = DamageSpecHandle;
    HitboxCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    
    if (GetInstigator())
    {
        HitboxCollisionBox->IgnoreActorWhenMoving(GetInstigator(), true);
    }

    bHasInitialized = true;
}

void AMortisHitboxBase::OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    APawn* HitPawn = Cast<APawn>(OtherActor);
    if (!HitPawn)
    {
        return;
    }

    if (HitActors.Contains(OtherActor))
    {
        return;
    }

    HitActors.AddUnique(OtherActor);
    BP_OnHitboxOverlappedFX(SweepResult.ImpactPoint);

    if (!HitboxDamageEffectSpecHandle.IsValid())
    {
        MORTIS_LOG("HitboxDamageEffectSpecHandle is invalid on %s", *GetActorNameOrLabel());
        return;
    }

    FGameplayEventData Payload;
    Payload.Instigator = GetInstigator();
    Payload.Target = HitPawn;
    Payload.OptionalObject = this;

    const bool bShouldDestroy = BP_HandleHitboxOverlap(HitPawn, SweepResult, Payload);

    if (bShouldDestroy && bDestroyOnOverlap)
    {
        PrepareDestroy();
    }
}

bool AMortisHitboxBase::BP_HandleHitboxOverlap_Implementation(APawn* HitPawn, const FHitResult& HitResult, const FGameplayEventData& Payload)
{
    if (!HitboxDamageEffectSpecHandle.IsValid())
    {
        return false;
    }
    
    // temprorary
    if (!Cast<AMortisPlayerCharacter>(HitPawn))
    {
        return false;
    }
    
    IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(HitPawn);
    if (!AbilitySystemInterface)
    {
        return false;
    }
    UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
    if (!ASC)
    {
        return false;
    }
    
    ASC->ApplyGameplayEffectSpecToSelf(*HitboxDamageEffectSpecHandle.Data.Get());
    return true;
}

void AMortisHitboxBase::PrepareDestroy()
{
    HitboxCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (DestroyDelayAfterOverlap > 0.f)
    {
        SetLifeSpan(DestroyDelayAfterOverlap);
    }
    else
    {
        Destroy();
    }
}