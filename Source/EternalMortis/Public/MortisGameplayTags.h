// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace MortisGameplayTags
{
	// ====================		Input Tags		====================
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);

	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Hold);

	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Hold_Ability_Run);
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Roll);

	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_LightAttack);

	// ====================		Ability Tags	====================	
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Run);
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Roll);

	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_EquipWeapon);
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_UnequipWeapon);

	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Light);

	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Attack_Melee);
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Attack_Ranged);
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Attack_Pattern);
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Turn);
	
	// ====================		State Tags		====================
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);

	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_Attacking);
	
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Turning);
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Strafing);

	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Enemy_Phase_1);
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Enemy_Phase_2);
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Enemy_Phase_3);
	
	// ====================		Event Tags		====================
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Player_SwitchWeapon);
	
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Request_ExecutePattern);
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Request_Turn);
	

	// ====================		Data Tags		====================
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Weapon_DefaultSword);

	// ====================		Character Tags	====================
}
