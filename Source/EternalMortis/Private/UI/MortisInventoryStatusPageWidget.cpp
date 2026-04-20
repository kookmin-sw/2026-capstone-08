#include "UI/MortisInventoryStatusPageWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MortisAttributeSet.h"
#include "AbilitySystem/Attributes/MortisPlayerAttributeSet.h"
#include "Character/Player/MortisPlayerCharacter.h"
#include "Components/Combat/MortisPlayerCombatComponent.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Items/Weapons/MortisPlayerWeapon.h"
#include "System/MortisRuneDatabaseSubsystem.h"
#include "System/MortisRuneInventorySubsystem.h"
#include "System/MortisWeaponInventorySubsystem.h"
#include "UObject/FieldIterator.h"
#include "UObject/UnrealType.h"

namespace
{
	bool IsNearlyWholeNumber(float Value)
	{
		return FMath::IsNearlyEqual(Value, FMath::RoundToFloat(Value), 0.001f);
	}

	float NormalizeStatusNumber(float Value)
	{
		return FMath::IsNearlyZero(Value, 0.001f) ? 0.0f : Value;
	}
}

void UMortisInventoryStatusPageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshStatusData();
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
