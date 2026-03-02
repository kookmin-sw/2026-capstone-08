// Fill out your copyright notice in the Description page of Project Settings.


#include "MortisGameplayTags.h"

namespace MortisGameplayTags
{
	// ====================		Input Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Hold, "InputTag.Hold");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Hold_Ability_Run, "InputTag.Hold.Ability.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Roll, "InputTag.Ability.Roll");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_LightAttack, "InputTag.Ability.LightAttack");

	// ====================		Ability Tags	====================
	UE_DEFINE_GAMEPLAY_TAG(Ability_Run, "Ability.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Roll, "Ability.Roll");

	UE_DEFINE_GAMEPLAY_TAG(Ability_EquipWeapon, "Ability.EquipWeapon");
	UE_DEFINE_GAMEPLAY_TAG(Ability_UnequipWeapon, "Ability.UnequipWeapon");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Light, "Ability.Attack.Light");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack_Melee, "Ability.Action.Attack.Melee");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack_Ranged, "Ability.Action.Attack.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack_Pattern, "Ability.Action.Attack.Pattern");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Turn, "Ability.Action.Turn");

	// ====================		State Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");

	UE_DEFINE_GAMEPLAY_TAG(State_Action_Attacking, "State.Action.Attacking");

	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Turning, "State.Movement.Turning");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Strafing, "State.Movement.Strafing");
	
	UE_DEFINE_GAMEPLAY_TAG(State_Enemy_Phase_1, "State.Enemy.Phase.1");
	UE_DEFINE_GAMEPLAY_TAG(State_Enemy_Phase_2, "State.Enemy.Phase.2");
	UE_DEFINE_GAMEPLAY_TAG(State_Enemy_Phase_3, "State.Enemy.Phase.3");
	// ====================		Event Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(Event_Player_SwitchWeapon, "Event.Player.SwitchWeapon");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Request_ExecutePattern, "Event.Request.ExecutePattern");
	UE_DEFINE_GAMEPLAY_TAG(Event_Request_Turn, "Event.Request.Turn");

	// ====================		Data Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(Data_Weapon_DefaultSword, "Data.Weapon.DefaultSword");

	// ====================		Character Tags	====================
}