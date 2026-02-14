// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogMortis, Log, All);

#define MORTIS_LOG(Msg, ...) \
	Debug::Print(FString::Printf(TEXT("%s (%s: Line: %d)"), \
	*FString::Printf(TEXT(Msg), ##__VA_ARGS__), \
	*FString(__FUNCTION__), __LINE__))

namespace Debug
{

	static void Print(const FString& Msg, const FColor& Color = FColor::MakeRandomColor(), int Key = -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(Key, 6.f, Color, Msg);
		}
		UE_LOG(LogMortis, Warning, TEXT("%s"), *Msg);
	}
}