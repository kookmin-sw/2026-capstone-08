// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawn/MortisRoomEnemySpawner.h"

#include "MortisDebugHelper.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Character/Player/MortisPlayerCharacter.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"

AMortisRoomEnemySpawner::AMortisRoomEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
	
	SpriteComponent = CreateDefaultSubobject<UBillboardComponent>("SpriteComponent");
	SpriteComponent->SetupAttachment(SceneRoot);
	// SpriteComponent->bIsScreenSizeScaled = true;
	
	SpawnTriggerBox = CreateDefaultSubobject<UBoxComponent>("SpawnTriggerBox");
	SpawnTriggerBox->SetupAttachment(SceneRoot);
	SpawnTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnTriggerOverlap);
}

void AMortisRoomEnemySpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
#if WITH_EDITOR
	DrawDebugPoints();
#endif
}

bool AMortisRoomEnemySpawner::ShouldTickIfViewportsOnly() const
{
	return true;
}

void AMortisRoomEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GIsEditor && !GetWorld()->IsGameWorld())
	{
#if WITH_EDITOR
		DrawDebugPoints();
#endif
	}
}

void AMortisRoomEnemySpawner::GenerateSpawnPoints()
{
	ClearPoints();
	switch (SpawnPattern)
	{
	case EMortisSpawnPattern::Point:
		GeneratePoint();
		break;
	case EMortisSpawnPattern::Circle:
		GenerateCircle();
		break;
	case EMortisSpawnPattern::Grid:
		GenerateGrid();
		break;
	case EMortisSpawnPattern::Custom:
		break;
	}
}

void AMortisRoomEnemySpawner::GeneratePoint()
{
	RelativeSpawnTransforms.Add(FTransform(FVector::ZeroVector));
}

void AMortisRoomEnemySpawner::GenerateCircle()
{
	RelativeSpawnTransforms.Add(FTransform(FVector::ZeroVector));
	for (int32 i = 0; i < CircleCount; i++)
	{
		const float Angle = 360.f / CircleCount * i;
		const float Rad = FMath::DegreesToRadians(Angle);
		const FVector RelativeLocation(FMath::Cos(Rad) * CircleRadius, FMath::Sin(Rad) * CircleRadius, 0.f);
		
		RelativeSpawnTransforms.Add(FTransform(RelativeLocation));
	}
}

void AMortisRoomEnemySpawner::GenerateGrid()
{
	for (int32 r = 0; r < GridRows; r++)
	{
		for (int32 c = 0; c < GridCols; c++)
		{
			const FVector RelativeLocation(r * GridSpacing, c * GridSpacing, 0.f);
			RelativeSpawnTransforms.Add(FTransform(RelativeLocation));
		}
	}
}

void AMortisRoomEnemySpawner::ClearPoints()
{
	RelativeSpawnTransforms.Empty();
}

TArray<FTransform> AMortisRoomEnemySpawner::GetWorldSpawnTransforms() const
{
	TArray<FTransform> WorldTransforms;
	for (const FTransform& RelativeTransform : RelativeSpawnTransforms)
	{
		// Unreal에서는 Row-Major이다. 행벡터를 사용하기 때문에 변환 행렬을 뒤에 곱해준다.
		WorldTransforms.Add(RelativeTransform * GetActorTransform());
	}
	return WorldTransforms;
}

void AMortisRoomEnemySpawner::SetEnemiesToSpawn(const TArray<TSubclassOf<AMortisEnemyCharacter>>& NewEnemiesToSpawn)
{
	EnemiesToSpawn = NewEnemiesToSpawn;
}

void AMortisRoomEnemySpawner::SpawnEnemies() const
{
	// MORTIS_LOG("Spawn Enemies");
	TArray<FTransform> WorldTransforms = GetWorldSpawnTransforms();
	for (int32 i = 0; i < WorldTransforms.Num(); i++)
	{
		if (!WorldTransforms.IsValidIndex(i) || !EnemiesToSpawn.IsValidIndex(i))
		{
			continue;
		}
		if (!WorldTransforms[i].IsValid() || !EnemiesToSpawn[i])
		{
			continue;
		}
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		GetWorld()->SpawnActor<AMortisEnemyCharacter>(EnemiesToSpawn[i], WorldTransforms[i], SpawnParams);
	}
}

void AMortisRoomEnemySpawner::OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// if (bHasSpawned)
	// {
	// 	return;
	// }
	if (AMortisPlayerCharacter* PlayerCharacter = Cast<AMortisPlayerCharacter>(OtherActor))
	{
		// MORTIS_LOG("%s: Trigger Box", *GetActorNameOrLabel());
		// bHasSpawned = true;
		
		SpawnTriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetActorTickEnabled(false);
		SpawnEnemies();
	}
}

#if WITH_EDITOR
void AMortisRoomEnemySpawner::DrawDebugPoints()
{
	if (!GetWorld())
	{
		return;
	}
	FlushPersistentDebugLines(GetWorld());
	
	for (const FTransform& SpawnTransform : RelativeSpawnTransforms)
	{
		FVector WorldLoc = GetActorTransform().TransformPosition(SpawnTransform.GetLocation());
		DrawDebugSphere(GetWorld(), WorldLoc, 10.f, 8, FColor::Cyan, false, -1.f, 0, 2.f);
	}
}
#endif