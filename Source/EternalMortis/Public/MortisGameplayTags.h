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

	// ====================		State Tags		====================
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);
	
	// ====================		Event Tags		====================
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Player_SwitchWeapon);

	// ====================		Data Tags		====================
	ETERNALMORTIS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Weapon_DefaultSword);

	// ====================		Character Tags	====================
}
