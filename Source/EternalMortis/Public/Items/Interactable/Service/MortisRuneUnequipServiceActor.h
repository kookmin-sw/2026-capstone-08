#pragma once

#include "CoreMinimal.h"
#include "Items/Interactable/Pickup/MortisPickupBase.h"
#include "MortisRuneUnequipServiceActor.generated.h"

class UMortisRuneUnequipWidget;

UCLASS()
class ETERNALMORTIS_API AMortisRuneUnequipServiceActor : public AMortisPickupBase
{
	GENERATED_BODY()

public:
	AMortisRuneUnequipServiceActor();

	virtual bool CanInteract(APawn* InteractingPawn) const override;
	virtual bool CanBeInteractionCandidate(APawn* InteractingPawn) const override;

	UFUNCTION(BlueprintCallable, Category = "Mortis|RuneUnequip")
	bool OpenRuneUnequipScreen(APawn* InteractingPawn);

protected:
	virtual void BeginPlay() override;
	virtual bool BuildPickupPreviewData(FMortisPickupPreviewData& OutPreviewData) const override;
	virtual void OnInteractionFinished(APawn* InteractingPawn, bool bSucceeded) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RuneUnequip")
	TSubclassOf<UMortisRuneUnequipWidget> RuneUnequipWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RuneUnequip")
	int32 WidgetZOrder = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RuneUnequip|Preview")
	FText PreviewTitle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RuneUnequip|Preview")
	FLinearColor PreviewAccentColor = FLinearColor(0.95f, 0.72f, 0.25f, 1.0f);

	UPROPERTY(Transient)
	TObjectPtr<UMortisRuneUnequipWidget> ActiveRuneUnequipWidget = nullptr;

private:
	void ConfigureInteractionCollision();
};
