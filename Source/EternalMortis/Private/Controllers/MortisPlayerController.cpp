// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/MortisPlayerController.h"

AMortisPlayerController::AMortisPlayerController()
{
	PlayerTeamID = FGenericTeamId(0);
}

FGenericTeamId AMortisPlayerController::GetGenericTeamId() const
{
	return PlayerTeamID;
}
