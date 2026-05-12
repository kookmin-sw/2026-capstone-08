// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/MortisPlayerController.h"
#include "Types/MortisEnumTypes.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/Widget.h"

void AMortisPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (!bShowMouseCursor)
	{
		GamepadCursorInput = FVector2D::ZeroVector;
		return;
	}

	if (GamepadCursorInput.IsNearlyZero(0.1f))
	{
		return;
	}

	float MouseX = 0.f;
	float MouseY = 0.f;

	if (!GetMousePosition(MouseX, MouseY))
	{
		return;
	}

	int32 ViewportX = 0;
	int32 ViewportY = 0;
	GetViewportSize(ViewportX, ViewportY);

	const float NewX = FMath::Clamp(
		MouseX + GamepadCursorInput.X * GamepadCursorSpeed * DeltaTime,
		0.f,
		static_cast<float>(ViewportX - 1)
	);

	const float NewY = FMath::Clamp(
		MouseY - GamepadCursorInput.Y * GamepadCursorSpeed * DeltaTime,
		0.f,
		static_cast<float>(ViewportY - 1)
	);

	SetMouseLocation(FMath::RoundToInt(NewX), FMath::RoundToInt(NewY));
}

AMortisPlayerController::AMortisPlayerController()
{
	PlayerTeamID = FGenericTeamId(static_cast<uint8>(EMortisTeam::Player));
}

void AMortisPlayerController::SetGamepadCursorInput(FVector2D InInput)
{
	GamepadCursorInput = InInput;
}

bool AMortisPlayerController::ClickHoveredButton()
{
	if (!bShowMouseCursor)
	{
		return false;
	}

	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
		GetWorld(),
		FoundWidgets,
		UUserWidget::StaticClass(),
		false
	);

	UButton* HoveredButton = nullptr;

	for (UUserWidget* UserWidget : FoundWidgets)
	{
		if (!IsValid(UserWidget) || !UserWidget->WidgetTree)
		{
			continue;
		}

		UserWidget->WidgetTree->ForEachWidget([&HoveredButton](UWidget* Widget)
			{
				if (UButton* Button = Cast<UButton>(Widget))
				{
					if (Button->IsHovered() && Button->GetIsEnabled())
					{
						HoveredButton = Button;
					}
				}
			});
	}

	if (!HoveredButton)
	{
		return false;
	}

	HoveredButton->OnClicked.Broadcast();
	return true;
}

FGenericTeamId AMortisPlayerController::GetGenericTeamId() const
{
	return PlayerTeamID;
}
