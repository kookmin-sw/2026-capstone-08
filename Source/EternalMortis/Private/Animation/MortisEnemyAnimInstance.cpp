// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MortisEnemyAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "MortisDebugHelper.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/MortisCharacterBase.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Character/Enemy/MortisEnemyData.h"
#include "Components/Combat/MortisCombatComponent.h"
#include "Controllers/MortisAIController.h"
#include "Items/Weapons/MortisWeaponBase.h"
#include "Kismet/KismetMathLibrary.h"

void UMortisEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwningCharacter)
	{
		if (UAbilitySystemComponent* ASC = OwningCharacter->GetAbilitySystemComponent())
		{
			BindTagMappings(ASC);
		}
	}

	if (AMortisEnemyCharacter* EnemyCharacter = Cast<AMortisEnemyCharacter>(OwningCharacter))
	{
		if (const UMortisEnemyData* EnemyData = EnemyCharacter->GetEnemyData())
		{
			SpeedInterpSpeed = EnemyData->SpeedInterpSpeed;
			DirectionInterpSpeed = EnemyData->DirectionInterpSpeed;

			bUseTwoHandedIK = EnemyData->bUseTwoHandedIK;
			LeftHandGripSocketName = EnemyData->LeftHandSocketName;
			
			LeftUpperArmSocketName = EnemyData->LeftUpperArmSocketName;
			ElbowBackOffset = EnemyData->ElbowBackOffset;
			ElbowOutOffset = EnemyData->ElbowOutOffset;
		}
	}
}

void UMortisEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (bUseTwoHandedIK)
	{
		UpdateIKAlpha(DeltaSeconds);
		UpdateLeftElbowJointTarget();
	}
	
	UpdateAimOffset(DeltaSeconds);
	
	if (UAnimMontage* CurrentMontage = GetCurrentActiveMontage())
	{
		float CurveSpeed = GetCurveValue(FName("PlayRateCurve"));
		if (CurveSpeed > 0.0f)
		{
			Montage_SetPlayRate(CurrentMontage, CurveSpeed);
		}
		else
		{
			Montage_SetPlayRate(CurrentMontage);
		}
	}
	
	// if (OwningCharacter)
	// {
	// 	FString DebugString = FString::Printf(TEXT("SmoothedSpeed: %f, SmoothedDirection: %f"), SmoothedSpeed, SmoothedDirection);
	// 	DrawDebugString(GetWorld(), OwningCharacter->GetActorLocation() + FVector(0.f, 0, 100.f), DebugString, OwningCharacter, FColor::Yellow, 0.f);
	// }
}

void UMortisEnemyAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void UMortisEnemyAnimInstance::UpdateAimOffset(float DeltaSeconds)
{
	if (!OwningCharacter)
	{
		return;
	}
	AMortisAIController* AIC = Cast<AMortisAIController>(OwningCharacter->GetController());
	if (!AIC)
	{
		return;
	}
	FRotator AimOffset = FRotator(AimPitch, AimYaw, 0.f);
	AActor* Target = AIC->GetTargetActor();
	if (!Target)
	{
		AimOffset = FMath::RInterpTo(AimOffset,
			FRotator::ZeroRotator,
			DeltaSeconds,
			AimOffsetInterpSpeed
		);
		AimPitch = FMath::ClampAngle(AimOffset.Pitch, -90.f, 90.f);
		AimYaw = FMath::ClampAngle(AimOffset.Yaw, -90.f, 90.f);
		return;
	}
	
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
		OwningCharacter->GetPawnViewLocation(),
		Target->GetActorLocation()
	);

	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(
		LookAtRot,
		OwningCharacter->GetActorRotation()
	);

	FRotator TargetAimOffset;
	TargetAimOffset.Pitch = FMath::ClampAngle(Delta.Pitch, -90.f, 90.f);
	TargetAimOffset.Yaw = FMath::ClampAngle(Delta.Yaw, -90.f, 90.f);
	
	AimOffset = FMath::RInterpTo(AimOffset,
		TargetAimOffset,
		DeltaSeconds,
		AimOffsetInterpSpeed
	);
	
	AimPitch = AimOffset.Pitch;
	AimYaw = AimOffset.Yaw;
	// MORTIS_LOG("AimPitch: %f, AimYaw: %f", AimPitch, AimYaw);
}

void UMortisEnemyAnimInstance::UpdateIKAlpha(float DeltaSeconds)
{
	UMortisCombatComponent* CombatComp = OwningCharacter->GetCombatComponent();
	if (!CombatComp)
	{
		return;
	}

	AMortisWeaponBase* Weapon = CombatComp->GetCurrentWeapon();
	if (!Weapon)
	{
		return;
	}
		
	UMeshComponent* WeaponMesh = Weapon->GetWeaponMesh();
	if (!WeaponMesh)
	{
		return;
	}

	float TargetAlpha = 0.f;
	if (WeaponMesh->DoesSocketExist(LeftHandGripSocketName))
	{
		FTransform SocketWorldTransform = WeaponMesh->GetSocketTransform(LeftHandGripSocketName, RTS_World);
		FTransform MeshComponentTransform = OwningCharacter->GetMesh()->GetComponentTransform();

		LeftHandIKTarget = SocketWorldTransform.GetRelativeTransform(MeshComponentTransform);
		
		TargetAlpha = 1.f;
	}
	
	CurrentIKAlpha = FMath::FInterpTo(CurrentIKAlpha, TargetAlpha, DeltaSeconds, 10.f);
}

void UMortisEnemyAnimInstance::UpdateLeftElbowJointTarget()
{
	if (!OwningCharacter)
	{
		return;
	}

	if (USkeletalMeshComponent* Mesh = OwningCharacter->GetMesh())
	{
		FTransform ShoulderTransform = Mesh->GetSocketTransform(LeftUpperArmSocketName, RTS_Component);
		
		FTransform MeshTransform = Mesh->GetComponentTransform();
		FVector CompLeft = MeshTransform.InverseTransformVector(OwningCharacter->GetActorRightVector() * -1.f);
		FVector CompBack = MeshTransform.InverseTransformVector(OwningCharacter->GetActorForwardVector() * -1.f);
		FVector ShoulderLoc = ShoulderTransform.GetLocation();
		
		LeftElbowJointTarget = ShoulderLoc + ElbowOutOffset * CompLeft + ElbowBackOffset * CompBack;
	}
}

void UMortisEnemyAnimInstance::BindTagMappings(UAbilitySystemComponent* ASC)
{
	for (const FMortisTagToBoolMapping& Mapping : TagToBoolMappings)
	{
		if (!Mapping.Tag.IsValid() || Mapping.BoolPropertyName.IsNone())
		{
			continue;
		}
		
		FBoolProperty* Prop = FindFProperty<FBoolProperty>(GetClass(), Mapping.BoolPropertyName);
		if (!Prop)
		{
			continue;
		}
		
		FOnGameplayEffectTagCountChanged::FDelegate Delegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(
			this,
			&ThisClass::OnTagChanged,
			Mapping.BoolPropertyName
		);
		
		ASC->RegisterAndCallGameplayTagEvent(
			Mapping.Tag,
			Delegate,	
			EGameplayTagEventType::NewOrRemoved
		);
	}
}

void UMortisEnemyAnimInstance::OnTagChanged(const FGameplayTag Tag, int32 NewCount, FName BoolPropertyName)
{
	if (FBoolProperty* Prop = FindFProperty<FBoolProperty>(GetClass(), BoolPropertyName))
	{
		Prop->SetPropertyValue_InContainer(this, NewCount > 0);
	}
}
