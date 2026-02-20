// Fill out your copyright notice in the Description page of Project Settings.


#include "MortisGameplayTags.h"

namespace MortisGameplayTags
{
	// ====================		Input Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Hold, "InputTag.Hold");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Hold_Ability_Run, "InputTag.Hold.Ability.Run");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Roll, "InputTag.Ability.Roll");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_LightAttack, "InputTag.Ability.LightAttack");

	// ====================		Ability Tags	====================
	UE_DEFINE_GAMEPLAY_TAG(Ability_Run, "Ability.Run");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Roll, "Ability.Roll");

	UE_DEFINE_GAMEPLAY_TAG(Ability_EquipWeapon, "Ability.EquipWeapon");
	UE_DEFINE_GAMEPLAY_TAG(Ability_UnequipWeapon, "Ability.UnequipWeapon");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Light, "Ability.Attack.Light");

	// ====================		State Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");

	// ====================		Event Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(Event_Player_SwitchWeapon, "Event.Player.SwitchWeapon");

	// ====================		Data Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(Data_Weapon_DefaultSword, "Data.Weapon.DefaultSword");

	// ====================		Character Tags	====================
}