#include "Spawn/MortisEnemySpawnerComponent.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Spawn/MortisEnemySpawnPointComponent.h"

#include "Components/BillboardComponent.h"

UMortisEnemySpawnerComponent::UMortisEnemySpawnerComponent()
{
#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("SpriteComponent"));
	SpriteComponent->SetupAttachment(this);
	SpriteComponent->SetRelativeLocation(FVector::ZeroVector);
#endif
}

void UMortisEnemySpawnerComponent::SetEnemiesToSpawn(const TArray<TSubclassOf<AMortisEnemyCharacter>>& NewEnemiesToSpawn)
{
	EnemiesToSpawn = NewEnemiesToSpawn;
}

TArray<AMortisEnemyCharacter*> UMortisEnemySpawnerComponent::SpawnEnemies() const
{
	TArray<UMortisEnemySpawnPointComponent*> SpawnPoints;
	if (GetOwner())
	{
		GetOwner()->GetComponents(UMortisEnemySpawnPointComponent::StaticClass(), SpawnPoints);
	}
	TArray<AMortisEnemyCharacter*> SpawnedEnemies;
	// TArray<FTransform> SpawnPoints = GetWorldSpawnTransforms();
	for (int32 i = 0; i < SpawnPoints.Num(); i++)
	{
		if (!SpawnPoints.IsValidIndex(i) || !EnemiesToSpawn.IsValidIndex(i))
		{
			continue;
		}
		if (!EnemiesToSpawn[i])
		{
			continue;
		}
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		if (AMortisEnemyCharacter* NewEnemy = GetWorld()->SpawnActor<AMortisEnemyCharacter>(EnemiesToSpawn[i], SpawnPoints[i]->GetComponentTransform(), SpawnParams))
		{
			SpawnedEnemies.Add(NewEnemy);
		}
	}
	return SpawnedEnemies;
}

#if WITH_EDITORONLY_DATA
void UMortisEnemySpawnerComponent::OnRegister()
{
	Super::OnRegister();
	
	if (SpriteComponent)
	{
		SpriteComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
		SpriteComponent->SetRelativeLocation(FVector::ZeroVector);
	}
}
#endif
