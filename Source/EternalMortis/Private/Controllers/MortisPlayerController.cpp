// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/MortisPlayerController.h"
#include "Types/MortisEnumTypes.h"

AMortisPlayerController::AMortisPlayerController()
{
	PlayerTeamID = FGenericTeamId(static_cast<uint8>(EMortisTeam::Player));
}

FGenericTeamId AMortisPlayerController::GetGenericTeamId() const
{
	return PlayerTeamID;
}
