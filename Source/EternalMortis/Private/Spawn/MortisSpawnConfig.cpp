#include "Spawn/MortisSpawnConfig.h"

#include "MortisDebugHelper.h"
#include "Character/MortisCharacterBase.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Projectile/MortisProjectileBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Spawn/MortisHitboxBase.h"
#include "Spawn/MortisSpawnIndicator.h"
#include "Spawn/MortisSpawnTypes.h"

struct FMortisSpawnEnemyRow;
struct FMortisRoomCustomSpawnRow;

void UMortisSpawnConfig::Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
}

void UMortisSpawnConfig::SpawnWithIndicatorOrImmediate(UWorld* World, TSubclassOf<AActor> ClassToSpawn, const FVector& Location, const FRotator& Rotation, const
    FGameplayEffectSpecHandle& DamageSpecHandle, APawn* Instigator)
{
    if (!World || !ClassToSpawn || !Instigator)
    {
        MORTIS_LOG("");
        return;
    }
    
    if (IndicatorClass)
    {
        FActorSpawnParameters Params;
        Params.Instigator = Instigator;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
        AMortisSpawnIndicator* Indicator = World->SpawnActor<AMortisSpawnIndicator>(IndicatorClass, Location, Rotation, Params);        
        if (Indicator)
        {
            Indicator->InitAndStart(ClassToSpawn, DamageSpecHandle, Instigator);
        }
    }
    else
    {
        FActorSpawnParameters Params;
        Params.Instigator = Instigator;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        
        AActor* Spawned = World->SpawnActor<AActor>(ClassToSpawn, Location, Rotation, Params);
        if (AMortisProjectileBase* Projectile = Cast<AMortisProjectileBase>(Spawned))
        {
            Projectile->InitializeProjectile(DamageSpecHandle);
        }
        else if (AMortisHitboxBase* Hitbox = Cast<AMortisHitboxBase>(Spawned))
        {
            Hitbox->InitializeHitbox(DamageSpecHandle);
        }
    }
}

void UMortisSpawnConfig_SingleHitbox::Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
    if (!Instigator || !Target || !HitboxClass)
    {
        return;
    }

    USkeletalMeshComponent* Mesh = Instigator->GetMesh();
    if (!Mesh)
    {
        MORTIS_LOG("Mesh is null!");
        return;
    }
    
    const FVector SpawnLocation = Mesh->GetSocketLocation(SpawnSocketName);
    const FVector Direction = (Target->GetActorLocation() - SpawnLocation).GetSafeNormal();
    const FRotator SpawnRotation = Direction.Rotation();

    SpawnWithIndicatorOrImmediate(Instigator->GetWorld(), HitboxClass, SpawnLocation, SpawnRotation, DamageSpecHandle, Instigator);
}

void UMortisSpawnConfig_SingleProjectile::Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
    if (!Instigator || !Target || !ProjectileClass)
    {
        return;
    }

    USkeletalMeshComponent* Mesh = Instigator->GetMesh();
    if (!Mesh)
    {
        MORTIS_LOG("Mesh is null!");
        return;
    }
    
    const FVector SpawnLocation = Mesh->GetSocketLocation(SpawnSocketName);
    const FVector Direction = (Target->GetActorLocation() - SpawnLocation).GetSafeNormal();
    const FRotator SpawnRotation = Direction.Rotation();

    SpawnWithIndicatorOrImmediate(Instigator->GetWorld(), ProjectileClass, SpawnLocation, SpawnRotation, DamageSpecHandle, Instigator);
}

void UMortisSpawnConfig_MultiProjectile::Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
    if (!Instigator || !Target || !ProjectileClass || ProjectileCount <= 0)
    {
        return;
    }

    USkeletalMeshComponent* Mesh = Instigator->GetMesh();
    if (!Mesh)
    {
        return;
    }
    
    const FVector SpawnLocation = Mesh->GetSocketLocation(SpawnSocketName);
    // const FVector BaseDirection = Instigator->GetActorForwardVector();
    const FVector BaseDirection = (Target->GetActorLocation() - SpawnLocation).GetSafeNormal();
    
    const float HalfSpread = SpreadAngleDegrees * 0.5f;
    const float AngleStep = (ProjectileCount > 1) ? SpreadAngleDegrees / (ProjectileCount - 1) : 0.f;
    
    for (int32 i = 0; i < ProjectileCount; i++)
    {
        const float Angle = (ProjectileCount > 1) ? -HalfSpread + AngleStep * i : 0.f;
        const FVector Direction = BaseDirection.RotateAngleAxis(Angle, FVector::UpVector);

        SpawnWithIndicatorOrImmediate(Instigator->GetWorld(), ProjectileClass, SpawnLocation, Direction.Rotation(), DamageSpecHandle, Instigator);
    }
}

void UMortisSpawnConfig_GroundSpawn::Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
    if (!Instigator || !Target || !EffectActorClass)
    {
        return;
    }
    
    FActorSpawnParameters Params;
    Params.Instigator = Instigator;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    for (const FVector& Offset : RelativeOffsets)
    {
        const FVector WorldOffset = Target->GetActorRotation().RotateVector(Offset);
        const FVector SpawnLocation = Target->GetActorLocation() + WorldOffset;

        SpawnWithIndicatorOrImmediate(Instigator->GetWorld(), EffectActorClass, SpawnLocation, FRotator::ZeroRotator, DamageSpecHandle, Instigator);
    }
}

void UMortisSpawnConfig_SummonEnemy::Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
    if (!Instigator || CustomEnemySpawnRowHandle.IsNull())
    {
        return;
    }

    FActorSpawnParameters Params;
    Params.Instigator = Instigator;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    FMortisRoomCustomSpawnRow* CustomSpawnRow = CustomEnemySpawnRowHandle.GetRow<FMortisRoomCustomSpawnRow>(TEXT("SpawnCustomSpawnRow"));
    if (!CustomSpawnRow)
    {
        return;
    }
	
    TArray<FDataTableRowHandle> EnemyRowHandles = CustomSpawnRow->EnemyRows;
    TArray<TSubclassOf<AMortisEnemyCharacter>> EnemiesToSpawn;
    for (const FDataTableRowHandle& EnemyRowHandle : EnemyRowHandles)
    {
        FMortisSpawnEnemyRow* EnemyRow = EnemyRowHandle.GetRow<FMortisSpawnEnemyRow>(TEXT("SpawnEnemyRow"));
        if (!EnemyRow)
        {
            continue;
        }
        EnemiesToSpawn.Add(EnemyRow->EnemyClass);
    }
    
    for (const TSubclassOf<AMortisEnemyCharacter>& EnemyClass : EnemiesToSpawn)
    {
        if (!EnemyClass)
        {
            continue;
        }
        const FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0.f, SpawnRadius);
        const FVector SpawnLocation = Instigator->GetActorLocation() + CircleCenterOffset + FVector(RandomOffset.X, RandomOffset.Y, 0.f);
        
        SpawnWithIndicatorOrImmediate(Instigator->GetWorld(), EnemyClass, SpawnLocation, Instigator->GetActorRotation(), DamageSpecHandle, Instigator);
    }
}