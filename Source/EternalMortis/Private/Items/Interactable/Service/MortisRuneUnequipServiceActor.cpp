#include "Items/Interactable/Service/MortisRuneUnequipServiceActor.h"

#include "AbilitySystem/Abilities/Player/Interact/MortisGA_OpenRuneUnequipService.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "UI/MortisPickupPreviewWidget.h"
#include "UI/MortisRuneUnequipWidget.h"

AMortisRuneUnequipServiceActor::AMortisRuneUnequipServiceActor()
{
	bDisableOnReserve = false;
	InteractionAbilityClass = UMortisGA_OpenRuneUnequipService::StaticClass();
	PreviewTitle = NSLOCTEXT("MortisRuneUnequip", "RuneRemovalServicePreviewTitle", "Rune Removal");

	ConfigureInteractionCollision();
}

bool AMortisRuneUnequipServiceActor::CanInteract(APawn* InteractingPawn) const
{
	return IsValid(InteractingPawn)
		&& Super::CanInteract(InteractingPawn)
		&& GetInteractionAbility() != nullptr
		&& RuneUnequipWidgetClass != nullptr;
}

bool AMortisRuneUnequipServiceActor::CanBeInteractionCandidate(APawn* InteractingPawn) const
{
	return CanInteract(InteractingPawn);
}

bool AMortisRuneUnequipServiceActor::OpenRuneUnequipScreen(APawn* InteractingPawn)
{
	if (!IsValid(InteractingPawn) || !RuneUnequipWidgetClass)
	{
		return false;
	}

	APlayerController* PlayerController = Cast<APlayerController>(InteractingPawn->GetController());
	if (!PlayerController)
	{
		return false;
	}

    if (IsValid(ActiveRuneUnequipWidget))
    {
        ActiveRuneUnequipWidget->RemoveFromParent();
        ActiveRuneUnequipWidget = nullptr;
    }

    ActiveRuneUnequipWidget = CreateWidget<UMortisRuneUnequipWidget>(PlayerController, RuneUnequipWidgetClass);

    if (!ActiveRuneUnequipWidget)
    {
        return false;
    }

    ActiveRuneUnequipWidget->AddToViewport(WidgetZOrder);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ActiveRuneUnequipWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
	ActiveRuneUnequipWidget->SetKeyboardFocus();

	return true;
}

void AMortisRuneUnequipServiceActor::BeginPlay()
{
	Super::BeginPlay();

	bDisableOnReserve = false;
	ConfigureInteractionCollision();
	EnableInteraction();
}

bool AMortisRuneUnequipServiceActor::BuildPickupPreviewData(FMortisPickupPreviewData& OutPreviewData) const
{
	OutPreviewData.TitleText = PreviewTitle.IsEmpty()
		? NSLOCTEXT("MortisRuneUnequip", "RuneRemovalServicePreviewFallbackTitle", "Rune Removal")
		: PreviewTitle;
	OutPreviewData.AccentColor = PreviewAccentColor;

	return !OutPreviewData.TitleText.IsEmpty();
}

void AMortisRuneUnequipServiceActor::OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded)
{
	if (bSucceeded)
	{
		OpenRuneUnequipScreen(InteractingPawn);
	}

	EnableInteraction();
}

void AMortisRuneUnequipServiceActor::ConfigureInteractionCollision()
{
	if (!InteractionCollision)
	{
		return;
	}

	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	// The player interaction sphere uses ECC_PhysicsBody, matching the existing pickup/chest setup.
	InteractionCollision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	InteractionCollision->SetGenerateOverlapEvents(true);
}
