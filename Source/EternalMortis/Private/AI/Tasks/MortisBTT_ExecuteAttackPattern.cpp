// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/MortisBTT_ExecuteAttackPattern.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "MortisDebugHelper.h"
#include "AbilitySystem/MortisAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Enemy/MortisGA_ExecuteAttackPattern.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Components/Combat/MortisEnemyCombatComponent.h"
#include "Controllers/MortisAIController.h"

UMortisBTT_ExecuteAttackPattern::UMortisBTT_ExecuteAttackPattern()
{
	NodeName = TEXT("Execute Attack Pattern");
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorKey), AActor::StaticClass());
	TargetDistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetDistanceKey));
}

void UMortisBTT_ExecuteAttackPattern::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetActorKey.ResolveSelectedKey(*BBAsset);
		TargetDistanceKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UMortisBTT_ExecuteAttackPattern::GetStaticDescription() const
{
	const FString KeyDescription = TargetActorKey.SelectedKeyName.ToString();

	return FString::Printf(TEXT("Execute attack pattern to attack %s."), *KeyDescription);
}

EBTNodeResult::Type UMortisBTT_ExecuteAttackPattern::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp)
	{
		MORTIS_LOG("BBComp is null");
		return EBTNodeResult::Failed;
	}
	float DistanceToTarget = BBComp->GetValueAsFloat(TargetDistanceKey.SelectedKeyName);
	// TODO: Calculate AngleToTarget
	float AngleToTarget = 0.f;
	
	AMortisAIController* AIC = Cast<AMortisAIController>(OwnerComp.GetAIOwner());
	if (!AIC)
	{
		MORTIS_LOG("AIC is null");
		return EBTNodeResult::Failed;
	}
	AMortisEnemyCharacter* EnemyCharacter = Cast<AMortisEnemyCharacter>(AIC->GetPawn());
	if (!EnemyCharacter)
	{
		MORTIS_LOG("Enemy is null");
		return EBTNodeResult::Failed;
	}
	UMortisEnemyCombatComponent* EnemyCombatComponent = EnemyCharacter->GetEnemyCombatComponent();
	if (!EnemyCombatComponent)
	{
		MORTIS_LOG("CombatComponent is null");
		return EBTNodeResult::Failed;
	}
	
	int32 PatternIndex = EnemyCombatComponent->SelectAttackPattern(DistanceToTarget, AngleToTarget);
	if (PatternIndex == INDEX_NONE)
	{
		MORTIS_LOG("PatternIndex is invalid");
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	FGameplayEventData Payload;
	Payload.Target = TargetActor;
	Payload.EventTag = MortisGameplayTags::Event_Request_ExecutePattern;
	Payload.EventMagnitude = static_cast<float>(PatternIndex);
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(EnemyCharacter, Payload.EventTag, Payload);
	CachedASC = EnemyCharacter->GetMortisAbilitySystemComponent();
	
	if (!CachedASC.IsValid())
	{
		MORTIS_LOG("ASC is null");
		return EBTNodeResult::Failed;
	}
	
	FGameplayAbilitySpec* Spec = nullptr;
	for (FGameplayAbilitySpec& AbilitySpec : CachedASC->GetActivatableAbilities())
	{
		if (/*AbilitySpec.IsActive() && */AbilitySpec.Ability && AbilitySpec.Ability->GetAssetTags().HasTagExact(MortisGameplayTags::Ability_Action_Attack_Pattern))
		{
			MORTIS_LOG("Spec Found");
			Spec = &AbilitySpec;
			break;
		}
	}
	
	if (!Spec)
	{
		MORTIS_LOG("Spec is null");
		return EBTNodeResult::Failed;
	}
	ActivatedHandle = Spec->Handle;
	CachedOwnerComp = &OwnerComp;
	
	EndDelegateHandle = CachedASC->OnAbilityEnded.AddUObject(this, &UMortisBTT_ExecuteAttackPattern::OnAbilityEnded);
	return EBTNodeResult::InProgress;
}

void UMortisBTT_ExecuteAttackPattern::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	if (CachedASC.IsValid() && EndDelegateHandle.IsValid())
	{
		CachedASC->OnAbilityEnded.Remove(EndDelegateHandle);
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UMortisBTT_ExecuteAttackPattern::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (AbilityEndedData.AbilitySpecHandle == ActivatedHandle)
	{
		if (CachedOwnerComp.IsValid())
		{
			FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
		}
	}
}


