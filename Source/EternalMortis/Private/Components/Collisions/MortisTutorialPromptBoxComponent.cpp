// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Collisions/MortisTutorialPromptBoxComponent.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UMortisTutorialPromptBoxComponent::UMortisTutorialPromptBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionObjectType(ECC_WorldDynamic);

	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SetGenerateOverlapEvents(true);
}

void UMortisTutorialPromptBoxComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UMortisTutorialPromptBoxComponent::OnPromptBoxBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UMortisTutorialPromptBoxComponent::OnPromptBoxEndOverlap);
}

void UMortisTutorialPromptBoxComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	HidePromptWidget();
	Super::EndPlay(EndPlayReason);
}

void UMortisTutorialPromptBoxComponent::OnPromptBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsPlayerActor(OtherActor))
		return;

	PlayerOverlapCount++;

	if (PlayerOverlapCount == 1)
		ShowPromptWidget();
}

void UMortisTutorialPromptBoxComponent::OnPromptBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsPlayerActor(OtherActor))
		return;

	PlayerOverlapCount = FMath::Max(0, PlayerOverlapCount - 1);

	if (PlayerOverlapCount == 0)
		HidePromptWidget();
}

bool UMortisTutorialPromptBoxComponent::IsPlayerActor(AActor* OtherActor) const
{
	if (!OtherActor)
		return false;

	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	return OtherActor == PlayerPawn;
}

void UMortisTutorialPromptBoxComponent::ShowPromptWidget()
{
	if (PromptWidgetInstance)
		return;

	if (!PromptWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PromptWidgetClass is not set on %s"), *GetName());
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)
		return;

	PromptWidgetInstance = CreateWidget<UUserWidget>(PlayerController, PromptWidgetClass);

	if (!PromptWidgetInstance)
		return;

	PromptWidgetInstance->AddToViewport(ZOrder);
}

void UMortisTutorialPromptBoxComponent::HidePromptWidget()
{
	if (!PromptWidgetInstance)
		return;

	PromptWidgetInstance->RemoveFromParent();
	PromptWidgetInstance = nullptr;
}
