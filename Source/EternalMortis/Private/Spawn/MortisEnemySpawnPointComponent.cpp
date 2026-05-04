// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawn/MortisEnemySpawnPointComponent.h"

#include "Components/BillboardComponent.h"

// Sets default values for this component's properties
UMortisEnemySpawnPointComponent::UMortisEnemySpawnPointComponent()
{
#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>("Sprite");
	if (SpriteComponent)
	{
		SpriteComponent->SetupAttachment(this);
		SpriteComponent->SetRelativeLocation(FVector::ZeroVector);
	}
#endif
}

#if WITH_EDITORONLY_DATA
void UMortisEnemySpawnPointComponent::OnRegister()
{
	Super::OnRegister();
	
	if (SpriteComponent)
	{
		SpriteComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
		SpriteComponent->SetRelativeLocation(FVector::ZeroVector);
	}
}
#endif