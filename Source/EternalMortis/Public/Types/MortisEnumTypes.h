// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class EMortisAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};

UENUM()
enum class EMortisValidType : uint8
{
	Valid,
	InValid
};

UENUM()
enum class EMortisSuccessType : uint8
{
	Success,
	Failed
};

UENUM()
enum class EMortisTeam : uint8
{
	Player = 0 UMETA(DisplayName = "Player"),
	Enemy = 1 UMETA(DisplayName = "Enemy"),
	Neutral = 2 UMETA(DisplayName = "Neutral"),
};

UENUM(BlueprintType)
enum class EMortisStatGrade : uint8
{
	S, A, B, C, D, F
};