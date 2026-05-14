#include "UI/MortisInventoryStatusPageWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MortisAttributeSet.h"
#include "AbilitySystem/Attributes/MortisPlayerAttributeSet.h"
#include "Character/Player/MortisPlayerCharacter.h"
#include "Components/Image.h"
#include "Components/Combat/MortisPlayerCombatComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Engine/GameInstance.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Items/Weapons/MortisPlayerWeapon.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "MortisFunctionLibrary.h"
#include "System/MortisRuneDatabaseSubsystem.h"
#include "System/MortisRuneInventorySubsystem.h"
#include "System/MortisWeaponDatabaseSubsystem.h"
#include "System/MortisWeaponInventorySubsystem.h"
#include "Styling/SlateBrush.h"
#include "UObject/FieldIterator.h"
#include "UObject/UnrealType.h"

namespace
{
	constexpr int32 WeaponPreviewRenderTargetSize = 1024;
	constexpr float WeaponPreviewFOV = 30.0f;
	constexpr float WeaponPreviewOrthoPadding = 1.22f;
	constexpr float WeaponPreviewMinOrthoWidth = 160.0f;
	constexpr float WeaponPreviewMinCameraDistance = 350.0f;
	const FName StatusWeaponPreviewStaticMeshName(TEXT("StatusWeaponPreviewStaticMesh"));
	const FName StatusWeaponPreviewSkeletalMeshName(TEXT("StatusWeaponPreviewSkeletalMesh"));
	const FName StatusWeaponPreviewCaptureName(TEXT("StatusWeaponPreviewCapture"));
	const FName StatusWeaponPreviewLightName(TEXT("StatusWeaponPreviewLight"));
	const FName WeaponPreviewTextureParameterName(TEXT("WeaponPreviewTexture"));

	bool IsNearlyWholeNumber(float Value)
	{
		return FMath::IsNearlyEqual(Value, FMath::RoundToFloat(Value), 0.001f);
	}

	float NormalizeStatusNumber(float Value)
	{
		return FMath::IsNearlyZero(Value, 0.001f) ? 0.0f : Value;
	}

	template <typename ComponentType>
	ComponentType* FindComponentByName(AActor* Owner, const FName ComponentName)
	{
		if (!Owner)
		{
			return nullptr;
		}

		TArray<ComponentType*> Components;
		Owner->GetComponents<ComponentType>(Components);
		for (ComponentType* Component : Components)
		{
			if (Component && Component->GetFName() == ComponentName)
			{
				return Component;
			}
		}

		return nullptr;
	}

	USceneComponent* GetOrCreatePreviewRoot(AActor* Owner)
	{
		if (!Owner)
		{
			return nullptr;
		}

		if (USceneComponent* ExistingRoot = Owner->GetRootComponent())
		{
			return ExistingRoot;
		}

		USceneComponent* NewRoot = NewObject<USceneComponent>(Owner, TEXT("StatusWeaponPreviewRoot"));
		if (!NewRoot)
		{
			return nullptr;
		}

		Owner->AddInstanceComponent(NewRoot);
		Owner->SetRootComponent(NewRoot);
		NewRoot->RegisterComponent();
		return NewRoot;
	}

	template <typename ComponentType>
	ComponentType* FindOrCreateAttachedComponent(AActor* Owner, const FName ComponentName)
	{
		if (ComponentType* ExistingComponent = FindComponentByName<ComponentType>(Owner, ComponentName))
		{
			return ExistingComponent;
		}

		USceneComponent* RootComponent = GetOrCreatePreviewRoot(Owner);
		if (!RootComponent)
		{
			return nullptr;
		}

		ComponentType* NewComponent = NewObject<ComponentType>(Owner, ComponentName);
		if (!NewComponent)
		{
			return nullptr;
		}

		Owner->AddInstanceComponent(NewComponent);
		NewComponent->SetupAttachment(RootComponent);
		NewComponent->RegisterComponent();
		return NewComponent;
	}
}

void UMortisInventoryStatusPageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshStatusData();
}

void UMortisInventoryStatusPageWidget::NativeDestruct()
{
	DestroyWeaponPreviewActor();

	Super::NativeDestruct();
}

void UMortisInventoryStatusPageWidget::RefreshStatusData(AActor* PlayerActor)
{
	CurrentStatusSnapshot = FMortisInventoryStatusSnapshot();

	AActor* ResolvedPlayerActor = ResolvePlayerActor(PlayerActor);
	CurrentStatusSnapshot.PlayerActor = ResolvedPlayerActor;
	CurrentStatusSnapshot.bHasPlayerActor = IsValid(ResolvedPlayerActor);

	CollectPlayerAttributes(ResolvedPlayerActor, CurrentStatusSnapshot);
	CollectWeaponInfo(ResolvedPlayerActor, CurrentStatusSnapshot);
	CollectRuneInfo(CurrentStatusSnapshot);
	RefreshWeaponPanel();
}

bool UMortisInventoryStatusPageWidget::HasWeaponInfo() const
{
	return CurrentStatusSnapshot.WeaponInfo.bHasWeaponRow
		|| CurrentStatusSnapshot.WeaponInfo.bHasEquippedWeaponActor;
}

bool UMortisInventoryStatusPageWidget::FindAttributeValueByName(FName AttributeName, FMortisInventoryStatusAttributeValue& OutValue) const
{
	for (const FMortisInventoryStatusAttributeValue& AttributeValue : CurrentStatusSnapshot.PlayerAttributes)
	{
		if (AttributeValue.AttributeName == AttributeName)
		{
			OutValue = AttributeValue;
			return true;
		}
	}

	OutValue = FMortisInventoryStatusAttributeValue();
	return false;
}

bool UMortisInventoryStatusPageWidget::FindRuneStatSummaryBySymbol(EMortisRuneSymbol SymbolType, FMortisInventoryStatusRuneStatSummary& OutSummary) const
{
	for (const FMortisInventoryStatusRuneStatSummary& RuneStatSummary : CurrentStatusSnapshot.EquippedRuneStatSummaries)
	{
		if (RuneStatSummary.SymbolType == SymbolType)
		{
			OutSummary = RuneStatSummary;
			return true;
		}
	}

	OutSummary = FMortisInventoryStatusRuneStatSummary();
	return false;
}

bool UMortisInventoryStatusPageWidget::FindRuneBonusValueBySymbol(EMortisRuneSymbol SymbolType, float& OutBonusValue) const
{
	FMortisInventoryStatusRuneStatSummary RuneStatSummary;
	if (FindRuneStatSummaryBySymbol(SymbolType, RuneStatSummary))
	{
		OutBonusValue = RuneStatSummary.TotalValue;
		return true;
	}

	OutBonusValue = 0.0f;
	return false;
}

bool UMortisInventoryStatusPageWidget::BuildAttributeTextByName(FName AttributeName, const FText LeftText, FText& OutLeftText, FText& OutRightText) const
{
	OutLeftText = LeftText;
	OutRightText = FText::GetEmpty();

	FMortisInventoryStatusAttributeValue AttributeValue;
	if (!FindAttributeValueByName(AttributeName, AttributeValue))
	{
		return false;
	}

	OutRightText = FormatStatusNumber(AttributeValue.Value);
	return true;
}

bool UMortisInventoryStatusPageWidget::BuildAttributeTextByRuneSymbol(EMortisRuneSymbol SymbolType, const FText LeftText, FText& OutLeftText, FText& OutRightText) const
{
	OutLeftText = LeftText;
	OutRightText = FText::GetEmpty();

	FName AttributeName;
	if (!GetAttributeNameByRuneSymbol(SymbolType, AttributeName))
	{
		return false;
	}

	FMortisInventoryStatusAttributeValue AttributeValue;
	if (!FindAttributeValueByName(AttributeName, AttributeValue))
	{
		return false;
	}

	float RuneBonusValue = 0.0f;
	FindRuneBonusValueBySymbol(SymbolType, RuneBonusValue);

	OutRightText = FormatStatusValueWithRuneBonus(AttributeValue.Value, RuneBonusValue);
	return true;
}

bool UMortisInventoryStatusPageWidget::BuildResourceTextByRuneSymbol(EMortisRuneSymbol SymbolType, const FText LeftText, FText& OutLeftText, FText& OutRightText) const
{
	OutLeftText = LeftText;
	OutRightText = FText::GetEmpty();

	FName CurrentAttributeName;
	FName MaxAttributeName;
	if (!GetResourceAttributeNamesByRuneSymbol(SymbolType, CurrentAttributeName, MaxAttributeName))
	{
		return false;
	}

	FMortisInventoryStatusAttributeValue CurrentAttributeValue;
	FMortisInventoryStatusAttributeValue MaxAttributeValue;
	if (!FindAttributeValueByName(CurrentAttributeName, CurrentAttributeValue) || !FindAttributeValueByName(MaxAttributeName, MaxAttributeValue))
	{
		return false;
	}

	float RuneBonusValue = 0.0f;
	FindRuneBonusValueBySymbol(SymbolType, RuneBonusValue);

	const FText ResourceValueText = FText::Format(
		NSLOCTEXT("MortisInventoryStatus", "ResourceValueFormat", "{0} / {1}"),
		FormatStatusNumber(CurrentAttributeValue.Value),
		FormatStatusNumber(MaxAttributeValue.Value));

	if (FMath::IsNearlyZero(RuneBonusValue, 0.001f))
	{
		OutRightText = ResourceValueText;
	}
	else
	{
		OutRightText = FText::Format(
			NSLOCTEXT("MortisInventoryStatus", "ResourceValueWithBonusFormat", "{0} ({1})"),
			ResourceValueText,
			FormatRuneBonus(RuneBonusValue));
	}

	return true;
}

AActor* UMortisInventoryStatusPageWidget::ResolvePlayerActor(AActor* ExplicitPlayerActor) const
{
	if (IsValid(ExplicitPlayerActor))
	{
		return ExplicitPlayerActor;
	}

	if (APawn* OwningPlayerPawn = GetOwningPlayerPawn())
	{
		return OwningPlayerPawn;
	}

	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			return PlayerController->GetPawn();
		}
	}

	return nullptr;
}

void UMortisInventoryStatusPageWidget::CollectPlayerAttributes(AActor* PlayerActor, FMortisInventoryStatusSnapshot& OutSnapshot) const
{
	if (!IsValid(PlayerActor))
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerActor);
	OutSnapshot.bHasAbilitySystem = IsValid(AbilitySystemComponent);

	if (!AbilitySystemComponent)
	{
		return;
	}

	for (TFieldIterator<FProperty> PropertyIt(UMortisPlayerAttributeSet::StaticClass(), EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
	{
		FProperty* AttributeProperty = *PropertyIt;
		const FStructProperty* StructProperty = CastField<FStructProperty>(AttributeProperty);
		if (!StructProperty || StructProperty->Struct != FGameplayAttributeData::StaticStruct())
		{
			continue;
		}

		UClass* OwnerClass = AttributeProperty->GetOwnerClass();
		if (!OwnerClass || !OwnerClass->IsChildOf(UMortisAttributeSet::StaticClass()))
		{
			continue;
		}

		const FGameplayAttribute Attribute(AttributeProperty);
		if (!Attribute.IsValid())
		{
			continue;
		}

		FMortisInventoryStatusAttributeValue AttributeValue;
		AttributeValue.Attribute = Attribute;
		AttributeValue.AttributeName = AttributeProperty->GetFName();
		AttributeValue.AttributeSetClass = OwnerClass;
		AttributeValue.AttributeSetName = OwnerClass->GetFName();
		AttributeValue.Value = AbilitySystemComponent->GetNumericAttribute(Attribute);

#if WITH_EDITOR
		AttributeValue.DisplayName = AttributeProperty->GetDisplayNameText();

		const FString CategoryString = AttributeProperty->GetMetaData(TEXT("Category"));
		AttributeValue.CategoryName = CategoryString.IsEmpty()
			? OwnerClass->GetFName()
			: FName(*CategoryString);
#else
		AttributeValue.DisplayName = FText::FromName(AttributeProperty->GetFName());
		AttributeValue.CategoryName = OwnerClass->GetFName();
#endif

		OutSnapshot.PlayerAttributes.Add(AttributeValue);
	}
}

void UMortisInventoryStatusPageWidget::CollectWeaponInfo(AActor* PlayerActor, FMortisInventoryStatusSnapshot& OutSnapshot) const
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (const UMortisWeaponInventorySubsystem* WeaponInventorySubsystem = GameInstance->GetSubsystem<UMortisWeaponInventorySubsystem>())
		{
			OutSnapshot.WeaponInfo.bHasWeaponRow = WeaponInventorySubsystem->HasWeapon();
			OutSnapshot.WeaponInfo.WeaponRow = WeaponInventorySubsystem->GetCurrentWeapon();
		}
	}

	if (!IsValid(PlayerActor))
	{
		return;
	}

	UMortisPlayerCombatComponent* PlayerCombatComponent = nullptr;
	if (const AMortisPlayerCharacter* PlayerCharacter = Cast<AMortisPlayerCharacter>(PlayerActor))
	{
		PlayerCombatComponent = PlayerCharacter->GetMortisPlayerCombatComponent();
	}

	if (!PlayerCombatComponent)
	{
		PlayerCombatComponent = PlayerActor->FindComponentByClass<UMortisPlayerCombatComponent>();
	}

	if (!PlayerCombatComponent)
	{
		return;
	}

	AMortisPlayerWeapon* EquippedWeapon = PlayerCombatComponent->GetPlayerCurrentEquippedWeapon();
	OutSnapshot.WeaponInfo.EquippedWeaponActor = EquippedWeapon;
	OutSnapshot.WeaponInfo.bHasEquippedWeaponActor = IsValid(EquippedWeapon);

	if (!OutSnapshot.WeaponInfo.bHasWeaponRow)
	{
		if (const UGameInstance* GameInstance = GetGameInstance())
		{
			if (const UMortisWeaponDatabaseSubsystem* WeaponDatabaseSubsystem = GameInstance->GetSubsystem<UMortisWeaponDatabaseSubsystem>())
			{
				FMortisWeaponRow ResolvedWeaponRow;
				const bool bResolvedByTag = PlayerCombatComponent->CurrentEquippedWeaponTag.IsValid()
					&& WeaponDatabaseSubsystem->GetWeaponRowByTag(PlayerCombatComponent->CurrentEquippedWeaponTag, ResolvedWeaponRow);
				const bool bResolvedByClass = !bResolvedByTag
					&& EquippedWeapon
					&& WeaponDatabaseSubsystem->GetWeaponRowByClass(EquippedWeapon->GetClass(), ResolvedWeaponRow);

				if (bResolvedByTag || bResolvedByClass)
				{
					OutSnapshot.WeaponInfo.bHasWeaponRow = true;
					OutSnapshot.WeaponInfo.WeaponRow = ResolvedWeaponRow;
				}
			}
		}
	}

	if (!EquippedWeapon)
	{
		return;
	}

	OutSnapshot.WeaponInfo.PlayerWeaponData = EquippedWeapon->PlayerWeaponData;
	OutSnapshot.WeaponInfo.EquippedWeaponMesh = EquippedWeapon->GetWeaponMesh();
	OutSnapshot.WeaponInfo.bHasEquippedWeaponMesh = IsValid(OutSnapshot.WeaponInfo.EquippedWeaponMesh);
}

void UMortisInventoryStatusPageWidget::CollectRuneInfo(FMortisInventoryStatusSnapshot& OutSnapshot) const
{
	const UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	const UMortisRuneInventorySubsystem* RuneInventorySubsystem = GameInstance->GetSubsystem<UMortisRuneInventorySubsystem>();
	const UMortisRuneDatabaseSubsystem* RuneDatabaseSubsystem = GameInstance->GetSubsystem<UMortisRuneDatabaseSubsystem>();
	if (!RuneInventorySubsystem)
	{
		return;
	}

	OutSnapshot.EquippedRunes = RuneInventorySubsystem->GetEquippedRunes();
	OutSnapshot.ActiveRuneSets = RuneInventorySubsystem->GetActiveRuneSets();

	TMap<FGameplayTag, int32> EquippedCountBySet;
	TMap<EMortisRuneSymbol, int32> StatSummaryIndexBySymbol;

	for (const FMortisRuneInstance& EquippedRune : OutSnapshot.EquippedRunes)
	{
		if (!EquippedRune.InstanceId.IsValid())
		{
			continue;
		}

		if (EquippedRune.SetTag.IsValid())
		{
			++EquippedCountBySet.FindOrAdd(EquippedRune.SetTag);
		}

		if (EquippedRune.SymbolType == EMortisRuneSymbol::None)
		{
			continue;
		}

		int32* ExistingSummaryIndex = StatSummaryIndexBySymbol.Find(EquippedRune.SymbolType);
		if (!ExistingSummaryIndex)
		{
			FMortisInventoryStatusRuneStatSummary NewSummary;
			NewSummary.SymbolType = EquippedRune.SymbolType;

			if (RuneDatabaseSubsystem)
			{
				if (const FMortisRuneSymbolRow* SymbolRow = RuneDatabaseSubsystem->GetRuneSymbolRow(EquippedRune.SymbolType))
				{
					NewSummary.bHasRuneSymbolRow = true;
					NewSummary.RuneSymbolRow = *SymbolRow;
				}
			}

			ExistingSummaryIndex = &StatSummaryIndexBySymbol.Add(
				EquippedRune.SymbolType,
				OutSnapshot.EquippedRuneStatSummaries.Add(NewSummary)
			);
		}

		FMortisInventoryStatusRuneStatSummary& Summary = OutSnapshot.EquippedRuneStatSummaries[*ExistingSummaryIndex];
		++Summary.RuneCount;
		Summary.TotalValue += EquippedRune.RolledValue;
		Summary.SourceRunes.Add(EquippedRune);
	}

	TArray<FGameplayTag> EquippedSetTags;
	EquippedCountBySet.GetKeys(EquippedSetTags);
	EquippedSetTags.Sort(
		[](const FGameplayTag& Left, const FGameplayTag& Right)
		{
			return Left.ToString() < Right.ToString();
		}
	);

	for (const FGameplayTag& SetTag : EquippedSetTags)
	{
		FMortisInventoryStatusRuneSetInfo SetInfo;
		SetInfo.SetTag = SetTag;
		SetInfo.EquippedCount = EquippedCountBySet.FindRef(SetTag);
		SetInfo.bIsActive = OutSnapshot.ActiveRuneSets.Contains(SetTag);

		if (RuneDatabaseSubsystem)
		{
			if (const FMortisRuneSetRow* SetRow = RuneDatabaseSubsystem->GetRuneSetRow(SetTag))
			{
				SetInfo.bHasRuneSetRow = true;
				SetInfo.RuneSetRow = *SetRow;
			}
		}

		FMortisActiveRuneSetState RuntimeState;
		if (RuneInventorySubsystem->GetRuneSetStateByTag(SetTag, RuntimeState))
		{
			SetInfo.bHasRuntimeState = true;
			SetInfo.ActiveRuneSetState = RuntimeState;
			SetInfo.CurrentLevel = RuntimeState.CurrentLevel;
		}

		OutSnapshot.EquippedRuneSetInfos.Add(SetInfo);
	}
}

void UMortisInventoryStatusPageWidget::RefreshWeaponPanel()
{
	const bool bHasWeapon = HasWeaponInfo();

	if (Text_WeaponName)
	{
		Text_WeaponName->SetText(bHasWeapon
			? ResolveWeaponNameText()
			: NSLOCTEXT("MortisInventoryStatus", "NoWeaponName", "No Weapon"));
	}

	if (Text_WeaponGrade)
	{
		Text_WeaponGrade->SetVisibility(bHasWeapon ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		if (bHasWeapon)
		{
			Text_WeaponGrade->SetText(ResolveWeaponGradeText());
			Text_WeaponGrade->SetColorAndOpacity(FSlateColor(ResolveWeaponGradeColor()));
		}
		else
		{
			Text_WeaponGrade->SetText(FText::GetEmpty());
		}
	}

	FMortisPlayerWeaponData WeaponData;
	const bool bHasWeaponData = bHasWeapon && ResolveDisplayWeaponData(WeaponData);
	const FText EmptyValue = NSLOCTEXT("MortisInventoryStatus", "EmptyWeaponStatValue", "-");

	if (Text_WeaponDamageValue)
	{
		Text_WeaponDamageValue->SetText(bHasWeaponData ? FormatStatusNumber(WeaponData.WeaponDamage) : EmptyValue);
	}

	if (Text_PoiseDamageValue)
	{
		Text_PoiseDamageValue->SetText(bHasWeaponData ? FormatStatusNumber(WeaponData.PoiseDamage) : EmptyValue);
	}

	if (Text_StrScalingValue)
	{
		Text_StrScalingValue->SetText(bHasWeaponData ? FormatScalingGrade(WeaponData.StrGrade) : EmptyValue);
	}

	if (Text_DexScalingValue)
	{
		Text_DexScalingValue->SetText(bHasWeaponData ? FormatScalingGrade(WeaponData.DexGrade) : EmptyValue);
	}

	if (Text_IntScalingValue)
	{
		Text_IntScalingValue->SetText(bHasWeaponData ? FormatScalingGrade(WeaponData.IntGrade) : EmptyValue);
	}

	RefreshWeaponPreviewImage(bHasWeapon);
}

void UMortisInventoryStatusPageWidget::RefreshWeaponPreviewImage(bool bHasWeapon)
{
	if (!bHasWeapon)
	{
		if (Image_WeaponPreview)
		{
			Image_WeaponPreview->SetVisibility(ESlateVisibility::Hidden);
		}
		DestroyWeaponPreviewActor();
		return;
	}

	const bool bHasPreview = RefreshWeaponPreviewActor();
	if (Image_WeaponPreview)
	{
		if (bHasPreview && WeaponPreviewRenderTarget)
		{
			if (UMaterialInstanceDynamic* PreviewMaterialInstance = GetOrCreateWeaponPreviewMaterialInstance())
			{
				PreviewMaterialInstance->SetTextureParameterValue(WeaponPreviewTextureParameterName, WeaponPreviewRenderTarget);

				FSlateBrush PreviewBrush;
				PreviewBrush.SetResourceObject(PreviewMaterialInstance);
				PreviewBrush.ImageSize = FVector2D(300.0f, 300.0f);
				Image_WeaponPreview->SetBrush(PreviewBrush);
				Image_WeaponPreview->SetVisibility(ESlateVisibility::Visible);
				return;
			}
		}

		Image_WeaponPreview->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UMortisInventoryStatusPageWidget::RefreshWeaponPreviewActor()
{
	UMeshComponent* SourcePreviewMesh = ResolveWeaponPreviewSourceMesh();
	UStaticMesh* SourceStaticMesh = nullptr;
	USkeletalMesh* SourceSkeletalMesh = nullptr;
	if (UStaticMeshComponent* SourceStaticMeshComponent = Cast<UStaticMeshComponent>(SourcePreviewMesh))
	{
		SourceStaticMesh = SourceStaticMeshComponent->GetStaticMesh();
	}
	else if (USkeletalMeshComponent* SourceSkeletalMeshComponent = Cast<USkeletalMeshComponent>(SourcePreviewMesh))
	{
		SourceSkeletalMesh = SourceSkeletalMeshComponent->GetSkeletalMeshAsset();
	}

	if (!SourceStaticMesh && !SourceSkeletalMesh)
	{
		DestroyWeaponPreviewActor();
		return false;
	}

	UWorld* World = GetWorld();
	TSubclassOf<AActor> PreviewActorClass = LoadWeaponPreviewActorClass();
	UTextureRenderTarget2D* PreviewRenderTarget = GetOrCreateWeaponPreviewRenderTarget();
	if (!World || !PreviewActorClass || !PreviewRenderTarget)
	{
		return false;
	}

	if (!IsValid(SpawnedWeaponPreviewActor))
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const FVector PreviewLocation(100000.0f, 100000.0f, 100000.0f);
		SpawnedWeaponPreviewActor = World->SpawnActor<AActor>(
			PreviewActorClass,
			PreviewLocation,
			FRotator::ZeroRotator,
			SpawnParameters);
	}

	if (!IsValid(SpawnedWeaponPreviewActor))
	{
		return false;
	}

	SpawnedWeaponPreviewActor->SetActorEnableCollision(false);

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	SpawnedWeaponPreviewActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (PrimitiveComponent)
		{
			PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	UStaticMeshComponent* PreviewMeshComponent = nullptr;
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	SpawnedWeaponPreviewActor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);
	for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
	{
		if (StaticMeshComponent && StaticMeshComponent->GetFName() == TEXT("PreviewMesh"))
		{
			PreviewMeshComponent = StaticMeshComponent;
			break;
		}
	}

	if (!PreviewMeshComponent && StaticMeshComponents.Num() > 0)
	{
		PreviewMeshComponent = StaticMeshComponents[0];
	}

	USkeletalMeshComponent* PreviewSkeletalMeshComponent = nullptr;
	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	SpawnedWeaponPreviewActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
	for (USkeletalMeshComponent* SkeletalMeshComponent : SkeletalMeshComponents)
	{
		if (SkeletalMeshComponent && SkeletalMeshComponent->GetFName() == TEXT("PreviewSkeletalMesh"))
		{
			PreviewSkeletalMeshComponent = SkeletalMeshComponent;
			break;
		}
	}

	const FTransform PreviewMeshTemplateTransform = PreviewMeshComponent
		? PreviewMeshComponent->GetComponentTransform()
		: SpawnedWeaponPreviewActor->GetActorTransform();
	UStaticMeshComponent* StatusPreviewStaticMeshComponent = FindOrCreateAttachedComponent<UStaticMeshComponent>(
		SpawnedWeaponPreviewActor,
		StatusWeaponPreviewStaticMeshName);
	USkeletalMeshComponent* StatusPreviewSkeletalMeshComponent = FindOrCreateAttachedComponent<USkeletalMeshComponent>(
		SpawnedWeaponPreviewActor,
		StatusWeaponPreviewSkeletalMeshName);
	if (!StatusPreviewStaticMeshComponent || !StatusPreviewSkeletalMeshComponent)
	{
		return false;
	}

	StatusPreviewStaticMeshComponent->SetWorldTransform(PreviewMeshTemplateTransform);
	StatusPreviewStaticMeshComponent->SetMobility(EComponentMobility::Movable);
	StatusPreviewStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StatusPreviewSkeletalMeshComponent->SetWorldTransform(PreviewMeshTemplateTransform);
	StatusPreviewSkeletalMeshComponent->SetMobility(EComponentMobility::Movable);
	StatusPreviewSkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PrimitiveComponents.Reset();
	SpawnedWeaponPreviewActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!PrimitiveComponent)
		{
			continue;
		}

		PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (PrimitiveComponent != StatusPreviewStaticMeshComponent && PrimitiveComponent != StatusPreviewSkeletalMeshComponent)
		{
			PrimitiveComponent->SetVisibility(false, true);
		}
	}

	UMeshComponent* ActivePreviewMeshComponent = nullptr;
	if (SourceStaticMesh)
	{
		StatusPreviewStaticMeshComponent->SetStaticMesh(SourceStaticMesh);
		StatusPreviewStaticMeshComponent->SetRelativeScale3D(FVector::OneVector);
		StatusPreviewStaticMeshComponent->SetVisibility(true, true);
		StatusPreviewStaticMeshComponent->SetHiddenInGame(false, true);
		StatusPreviewStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StatusPreviewStaticMeshComponent->MarkRenderStateDirty();
		ActivePreviewMeshComponent = StatusPreviewStaticMeshComponent;

		StatusPreviewSkeletalMeshComponent->SetSkeletalMesh(nullptr);
		StatusPreviewSkeletalMeshComponent->SetVisibility(false, true);

		if (PreviewSkeletalMeshComponent)
		{
			PreviewSkeletalMeshComponent->SetSkeletalMesh(nullptr);
			PreviewSkeletalMeshComponent->SetVisibility(false, true);
		}
	}
	else if (SourceSkeletalMesh)
	{
		StatusPreviewSkeletalMeshComponent->SetSkeletalMesh(SourceSkeletalMesh);
		StatusPreviewSkeletalMeshComponent->SetRelativeScale3D(FVector::OneVector);
		StatusPreviewSkeletalMeshComponent->SetVisibility(true, true);
		StatusPreviewSkeletalMeshComponent->SetHiddenInGame(false, true);
		StatusPreviewSkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StatusPreviewSkeletalMeshComponent->MarkRenderStateDirty();
		ActivePreviewMeshComponent = StatusPreviewSkeletalMeshComponent;

		StatusPreviewStaticMeshComponent->SetStaticMesh(nullptr);
		StatusPreviewStaticMeshComponent->SetVisibility(false, true);

		if (PreviewMeshComponent)
		{
			PreviewMeshComponent->SetStaticMesh(nullptr);
			PreviewMeshComponent->SetVisibility(false, true);
		}
	}

	if (!ActivePreviewMeshComponent)
	{
		return false;
	}

	USceneCaptureComponent2D* TemplateSceneCaptureComponent = nullptr;
	TArray<USceneCaptureComponent2D*> SceneCaptureComponents;
	SpawnedWeaponPreviewActor->GetComponents<USceneCaptureComponent2D>(SceneCaptureComponents);
	for (USceneCaptureComponent2D* SceneCaptureComponent : SceneCaptureComponents)
	{
		if (!SceneCaptureComponent)
		{
			continue;
		}

		if (SceneCaptureComponent->GetFName() == StatusWeaponPreviewCaptureName)
		{
			continue;
		}

		if (!TemplateSceneCaptureComponent)
		{
			TemplateSceneCaptureComponent = SceneCaptureComponent;
		}

		SceneCaptureComponent->bCaptureEveryFrame = false;
		SceneCaptureComponent->bCaptureOnMovement = false;
		SceneCaptureComponent->TextureTarget = nullptr;
		SceneCaptureComponent->ClearShowOnlyComponents();
	}

	USceneCaptureComponent2D* StatusSceneCaptureComponent = FindOrCreateAttachedComponent<USceneCaptureComponent2D>(
		SpawnedWeaponPreviewActor,
		StatusWeaponPreviewCaptureName);
	if (!StatusSceneCaptureComponent)
	{
		return false;
	}

	StatusSceneCaptureComponent->SetWorldRotation(TemplateSceneCaptureComponent
		? TemplateSceneCaptureComponent->GetComponentRotation()
		: FRotator::ZeroRotator);
	StatusSceneCaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	StatusSceneCaptureComponent->CaptureSource = SCS_SceneColorHDR;
	StatusSceneCaptureComponent->CompositeMode = SCCM_Overwrite;
	StatusSceneCaptureComponent->bCaptureEveryFrame = false;
	StatusSceneCaptureComponent->bCaptureOnMovement = false;
	StatusSceneCaptureComponent->TextureTarget = PreviewRenderTarget;
	StatusSceneCaptureComponent->ClearShowOnlyComponents();
	StatusSceneCaptureComponent->ShowOnlyActors.Reset();
	StatusSceneCaptureComponent->HiddenActors.Reset();
	TArray<FEngineShowFlagsSetting> PreviewShowFlagSettings;
	auto AddPreviewShowFlag = [&PreviewShowFlagSettings](const FString& ShowFlagName, bool bEnabled)
	{
		FEngineShowFlagsSetting ShowFlagSetting;
		ShowFlagSetting.ShowFlagName = ShowFlagName;
		ShowFlagSetting.Enabled = bEnabled;
		PreviewShowFlagSettings.Add(ShowFlagSetting);
	};
	AddPreviewShowFlag(TEXT("Atmosphere"), false);
	AddPreviewShowFlag(TEXT("Fog"), false);
	AddPreviewShowFlag(TEXT("SkyLighting"), false);
	AddPreviewShowFlag(TEXT("VolumetricFog"), false);
	AddPreviewShowFlag(TEXT("VolumetricCloud"), false);
	StatusSceneCaptureComponent->SetShowFlagSettings(PreviewShowFlagSettings);
	PreviewRenderTarget->ResizeTarget(WeaponPreviewRenderTargetSize, WeaponPreviewRenderTargetSize);
	StatusSceneCaptureComponent->ShowOnlyComponent(ActivePreviewMeshComponent);
	FitWeaponPreviewMeshToCapture(ActivePreviewMeshComponent, StatusSceneCaptureComponent);

	if (UPointLightComponent* StatusPreviewLightComponent = FindOrCreateAttachedComponent<UPointLightComponent>(
		SpawnedWeaponPreviewActor,
		StatusWeaponPreviewLightName))
	{
		const FVector LightLocation = ActivePreviewMeshComponent->Bounds.Origin
			- (StatusSceneCaptureComponent->GetForwardVector() * 160.0f)
			+ (StatusSceneCaptureComponent->GetUpVector() * 90.0f);
		StatusPreviewLightComponent->SetMobility(EComponentMobility::Movable);
		StatusPreviewLightComponent->SetWorldLocation(LightLocation);
		StatusPreviewLightComponent->SetIntensity(7000.0f);
		StatusPreviewLightComponent->SetAttenuationRadius(900.0f);
		StatusPreviewLightComponent->SetVisibility(true);
	}

	StatusSceneCaptureComponent->CaptureScene();
	return true;
}

void UMortisInventoryStatusPageWidget::DestroyWeaponPreviewActor()
{
	if (IsValid(SpawnedWeaponPreviewActor))
	{
		SpawnedWeaponPreviewActor->Destroy();
	}

	SpawnedWeaponPreviewActor = nullptr;
}

bool UMortisInventoryStatusPageWidget::ResolveDisplayWeaponData(FMortisPlayerWeaponData& OutWeaponData) const
{
	if (CurrentStatusSnapshot.WeaponInfo.bHasEquippedWeaponActor)
	{
		OutWeaponData = CurrentStatusSnapshot.WeaponInfo.PlayerWeaponData;
		return true;
	}

	if (!CurrentStatusSnapshot.WeaponInfo.bHasWeaponRow || CurrentStatusSnapshot.WeaponInfo.WeaponRow.WeaponClass.IsNull())
	{
		OutWeaponData = FMortisPlayerWeaponData();
		return false;
	}

	UClass* WeaponClass = CurrentStatusSnapshot.WeaponInfo.WeaponRow.WeaponClass.LoadSynchronous();
	if (const AMortisPlayerWeapon* PlayerWeaponCDO = WeaponClass ? Cast<AMortisPlayerWeapon>(WeaponClass->GetDefaultObject()) : nullptr)
	{
		OutWeaponData = PlayerWeaponCDO->PlayerWeaponData;
		return true;
	}

	OutWeaponData = FMortisPlayerWeaponData();
	return false;
}

FText UMortisInventoryStatusPageWidget::ResolveWeaponNameText() const
{
	const FMortisInventoryStatusWeaponInfo& WeaponInfo = CurrentStatusSnapshot.WeaponInfo;
	if (WeaponInfo.bHasWeaponRow && !WeaponInfo.WeaponRow.WeaponName.IsEmpty())
	{
		return WeaponInfo.WeaponRow.WeaponName;
	}

	return NSLOCTEXT("MortisInventoryStatus", "EquippedWeaponNameFallback", "Equipped Weapon");
}

FText UMortisInventoryStatusPageWidget::ResolveWeaponGradeText() const
{
	const UEnum* GradeEnum = StaticEnum<EMortisWeaponGrade>();
	return GradeEnum
		? GradeEnum->GetDisplayNameTextByValue(static_cast<int64>(CurrentStatusSnapshot.WeaponInfo.WeaponRow.Grade))
		: NSLOCTEXT("MortisInventoryStatus", "FallbackWeaponGrade", "Common");
}

FLinearColor UMortisInventoryStatusPageWidget::ResolveWeaponGradeColor() const
{
	const FMortisInventoryStatusWeaponInfo& WeaponInfo = CurrentStatusSnapshot.WeaponInfo;
	if (WeaponInfo.bHasWeaponRow && WeaponInfo.WeaponRow.GradeStyleRow.DataTable)
	{
		if (const FMortisWeaponGradeStyleRow* GradeStyleRow = WeaponInfo.WeaponRow.GradeStyleRow.GetRow<FMortisWeaponGradeStyleRow>(TEXT("StatusWeaponPanel")))
		{
			return GradeStyleRow->GradeColor;
		}
	}

	return FLinearColor::White;
}

UMeshComponent* UMortisInventoryStatusPageWidget::ResolveWeaponPreviewSourceMesh() const
{
	const FMortisInventoryStatusWeaponInfo& WeaponInfo = CurrentStatusSnapshot.WeaponInfo;
	if (UStaticMeshComponent* EquippedStaticMeshComponent = Cast<UStaticMeshComponent>(WeaponInfo.EquippedWeaponMesh.Get()))
	{
		if (EquippedStaticMeshComponent->GetStaticMesh())
		{
			return EquippedStaticMeshComponent;
		}
	}

	if (USkeletalMeshComponent* EquippedSkeletalMeshComponent = Cast<USkeletalMeshComponent>(WeaponInfo.EquippedWeaponMesh.Get()))
	{
		if (EquippedSkeletalMeshComponent->GetSkeletalMeshAsset())
		{
			return EquippedSkeletalMeshComponent;
		}
	}

	if (IsValid(WeaponInfo.EquippedWeaponActor))
	{
		TArray<UStaticMeshComponent*> StaticMeshComponents;
		WeaponInfo.EquippedWeaponActor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);
		for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
		{
			if (StaticMeshComponent && StaticMeshComponent->GetStaticMesh())
			{
				return StaticMeshComponent;
			}
		}

		TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
		WeaponInfo.EquippedWeaponActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
		for (USkeletalMeshComponent* SkeletalMeshComponent : SkeletalMeshComponents)
		{
			if (SkeletalMeshComponent && SkeletalMeshComponent->GetSkeletalMeshAsset())
			{
				return SkeletalMeshComponent;
			}
		}
	}

	if (WeaponInfo.bHasWeaponRow && !WeaponInfo.WeaponRow.WeaponClass.IsNull())
	{
		return ResolveWeaponPreviewSourceMeshFromWeaponClass(WeaponInfo.WeaponRow.WeaponClass.LoadSynchronous());
	}

	return nullptr;
}

UMeshComponent* UMortisInventoryStatusPageWidget::ResolveWeaponPreviewSourceMeshFromWeaponClass(UClass* WeaponClass) const
{
	const AActor* WeaponCDO = WeaponClass ? Cast<AActor>(WeaponClass->GetDefaultObject()) : nullptr;
	if (!WeaponCDO)
	{
		return nullptr;
	}

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	WeaponCDO->GetComponents<UStaticMeshComponent>(StaticMeshComponents);
	for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
	{
		if (StaticMeshComponent && StaticMeshComponent->GetStaticMesh())
		{
			return StaticMeshComponent;
		}
	}

	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	WeaponCDO->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
	for (USkeletalMeshComponent* SkeletalMeshComponent : SkeletalMeshComponents)
	{
		if (SkeletalMeshComponent && SkeletalMeshComponent->GetSkeletalMeshAsset())
		{
			return SkeletalMeshComponent;
		}
	}

	return nullptr;
}

UTextureRenderTarget2D* UMortisInventoryStatusPageWidget::GetOrCreateWeaponPreviewRenderTarget()
{
	if (!WeaponPreviewRenderTarget)
	{
		WeaponPreviewRenderTarget = NewObject<UTextureRenderTarget2D>(this, TEXT("StatusWeaponPreviewRenderTarget"));
		if (!WeaponPreviewRenderTarget)
		{
			return nullptr;
		}

		WeaponPreviewRenderTarget->RenderTargetFormat = RTF_RGBA16f;
		WeaponPreviewRenderTarget->ClearColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
		WeaponPreviewRenderTarget->InitAutoFormat(WeaponPreviewRenderTargetSize, WeaponPreviewRenderTargetSize);
		WeaponPreviewRenderTarget->UpdateResourceImmediate(true);
	}
	else if (WeaponPreviewRenderTarget->SizeX != WeaponPreviewRenderTargetSize
		|| WeaponPreviewRenderTarget->SizeY != WeaponPreviewRenderTargetSize)
	{
		WeaponPreviewRenderTarget->ResizeTarget(WeaponPreviewRenderTargetSize, WeaponPreviewRenderTargetSize);
	}

	return WeaponPreviewRenderTarget;
}

UMaterialInstanceDynamic* UMortisInventoryStatusPageWidget::GetOrCreateWeaponPreviewMaterialInstance()
{
	if (!WeaponPreviewMaterialInstance)
	{
		UMaterialInterface* PreviewMaterial = LoadStatusWeaponPreviewMaterial();
		if (!PreviewMaterial)
		{
			return nullptr;
		}

		WeaponPreviewMaterialInstance = UMaterialInstanceDynamic::Create(PreviewMaterial, this);
	}

	return WeaponPreviewMaterialInstance;
}

TSubclassOf<AActor> UMortisInventoryStatusPageWidget::LoadWeaponPreviewActorClass()
{
	return LoadClass<AActor>(nullptr, TEXT("/Game/UI/PreviewActor/BP_WeaponPreviewActor.BP_WeaponPreviewActor_C"));
}

UMaterialInterface* UMortisInventoryStatusPageWidget::LoadStatusWeaponPreviewMaterial()
{
	return LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/UI/PreviewActor/M_UI_StatusWeaponPreview.M_UI_StatusWeaponPreview"));
}

bool UMortisInventoryStatusPageWidget::FitWeaponPreviewMeshToCapture(UMeshComponent* PreviewMeshComponent, USceneCaptureComponent2D* SceneCaptureComponent)
{
	if (!PreviewMeshComponent || !SceneCaptureComponent)
	{
		return false;
	}

	PreviewMeshComponent->UpdateComponentToWorld();

	const FBoxSphereBounds MeshBounds = PreviewMeshComponent->Bounds;
	if (MeshBounds.SphereRadius <= UE_KINDA_SMALL_NUMBER)
	{
		return false;
	}

	const FVector ViewForward = SceneCaptureComponent->GetForwardVector().GetSafeNormal();
	const FVector ViewRight = SceneCaptureComponent->GetRightVector().GetSafeNormal();
	const FVector ViewUp = SceneCaptureComponent->GetUpVector().GetSafeNormal();
	if (ViewForward.IsNearlyZero() || ViewRight.IsNearlyZero() || ViewUp.IsNearlyZero())
	{
		return false;
	}

	const FVector BoundsExtent = MeshBounds.BoxExtent;
	const FVector AbsViewRight(FMath::Abs(ViewRight.X), FMath::Abs(ViewRight.Y), FMath::Abs(ViewRight.Z));
	const FVector AbsViewUp(FMath::Abs(ViewUp.X), FMath::Abs(ViewUp.Y), FMath::Abs(ViewUp.Z));
	const float ViewWidth = FVector::DotProduct(BoundsExtent, AbsViewRight) * 2.0f;
	const float ViewHeight = FVector::DotProduct(BoundsExtent, AbsViewUp) * 2.0f;
	const float RequiredOrthoWidth = FMath::Max3(ViewWidth, ViewHeight, WeaponPreviewMinOrthoWidth) * WeaponPreviewOrthoPadding;
	if (!FMath::IsFinite(RequiredOrthoWidth))
	{
		return false;
	}

	const float CameraDistance = FMath::Max(MeshBounds.SphereRadius * 2.5f, WeaponPreviewMinCameraDistance);
	SceneCaptureComponent->SetWorldLocation(MeshBounds.Origin - (ViewForward * CameraDistance));
	SceneCaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCaptureComponent->FOVAngle = WeaponPreviewFOV;
	SceneCaptureComponent->OrthoWidth = RequiredOrthoWidth;
	SceneCaptureComponent->bAutoCalculateOrthoPlanes = true;
	SceneCaptureComponent->bUpdateOrthoPlanes = true;
	SceneCaptureComponent->bUseCameraHeightAsViewTarget = false;
	SceneCaptureComponent->MaxViewDistanceOverride = FMath::Max(CameraDistance + MeshBounds.SphereRadius * 4.0f, 1000.0f);
	return true;
}

bool UMortisInventoryStatusPageWidget::GetAttributeNameByRuneSymbol(EMortisRuneSymbol SymbolType, FName& OutAttributeName)
{
	switch (SymbolType)
	{
	case EMortisRuneSymbol::MaxHealth:
		OutAttributeName = TEXT("MaxHealth");
		return true;
	case EMortisRuneSymbol::MaxStamina:
		OutAttributeName = TEXT("MaxStamina");
		return true;
	case EMortisRuneSymbol::StaminaHealSpeed:
		OutAttributeName = TEXT("StaminaRegen");
		return true;
	case EMortisRuneSymbol::MaxMana:
		OutAttributeName = TEXT("MaxMana");
		return true;
	case EMortisRuneSymbol::ManaHealSpeed:
		OutAttributeName = TEXT("ManaRegen");
		return true;
	case EMortisRuneSymbol::Strength:
		OutAttributeName = TEXT("Strength");
		return true;
	case EMortisRuneSymbol::Dexterity:
		OutAttributeName = TEXT("Dexterity");
		return true;
	case EMortisRuneSymbol::Intelligence:
		OutAttributeName = TEXT("Intelligence");
		return true;
	case EMortisRuneSymbol::Pierce:
		OutAttributeName = TEXT("PierceBonus");
		return true;
	case EMortisRuneSymbol::Strike:
		OutAttributeName = TEXT("BluntBonus");
		return true;
	case EMortisRuneSymbol::Slash:
		OutAttributeName = TEXT("SlashBonus");
		return true;
	case EMortisRuneSymbol::Magic:
		OutAttributeName = TEXT("MagicBonus");
		return true;
	default:
		OutAttributeName = NAME_None;
		return false;
	}
}

bool UMortisInventoryStatusPageWidget::GetResourceAttributeNamesByRuneSymbol(EMortisRuneSymbol SymbolType, FName& OutCurrentAttributeName, FName& OutMaxAttributeName)
{
	switch (SymbolType)
	{
	case EMortisRuneSymbol::MaxHealth:
		OutCurrentAttributeName = TEXT("CurrentHealth");
		OutMaxAttributeName = TEXT("MaxHealth");
		return true;
	case EMortisRuneSymbol::MaxStamina:
		OutCurrentAttributeName = TEXT("CurrentStamina");
		OutMaxAttributeName = TEXT("MaxStamina");
		return true;
	case EMortisRuneSymbol::MaxMana:
		OutCurrentAttributeName = TEXT("CurrentMana");
		OutMaxAttributeName = TEXT("MaxMana");
		return true;
	default:
		OutCurrentAttributeName = NAME_None;
		OutMaxAttributeName = NAME_None;
		return false;
	}
}

FText UMortisInventoryStatusPageWidget::FormatScalingGrade(EMortisStatGrade Grade)
{
	return FText::FromString(UMortisFunctionLibrary::GetGradeText(Grade));
}

FText UMortisInventoryStatusPageWidget::FormatStatusNumber(float Value)
{
	const float NormalizedValue = NormalizeStatusNumber(Value);

	FNumberFormattingOptions FormattingOptions;
	FormattingOptions.MinimumFractionalDigits = 0;
	FormattingOptions.MaximumFractionalDigits = IsNearlyWholeNumber(NormalizedValue) ? 0 : 2;
	FormattingOptions.UseGrouping = false;

	return FText::AsNumber(NormalizedValue, &FormattingOptions);
}

FText UMortisInventoryStatusPageWidget::FormatRuneBonus(float RuneBonusValue)
{
	const FText BonusText = FormatStatusNumber(FMath::Abs(RuneBonusValue));
	if (RuneBonusValue > 0.0f)
	{
		return FText::Format(
			NSLOCTEXT("MortisInventoryStatus", "PositiveRuneBonusFormat", "+{0}"),
			BonusText);
	}

	return FText::Format(
		NSLOCTEXT("MortisInventoryStatus", "NegativeRuneBonusFormat", "-{0}"),
		BonusText);
}

FText UMortisInventoryStatusPageWidget::FormatStatusValueWithRuneBonus(float Value, float RuneBonusValue)
{
	const FText ValueText = FormatStatusNumber(Value);
	if (FMath::IsNearlyZero(RuneBonusValue, 0.001f))
	{
		return ValueText;
	}

	return FText::Format(
		NSLOCTEXT("MortisInventoryStatus", "ValueWithRuneBonusFormat", "{0} ({1})"),
		ValueText,
		FormatRuneBonus(RuneBonusValue));
}
