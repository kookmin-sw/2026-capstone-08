// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM()
enum class EMortisTeam : uint8
{
	Player = 0 UMETA(DisplayName = "Player"),
	Enemy = 1 UMETA(DisplayName = "Enemy"),
	Neutral = 2 UMETA(DisplayName = "Neutral"),
};
