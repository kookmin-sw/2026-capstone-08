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
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_WeaponSkill, "InputTag.Ability.WeaponSkill");

	// ====================		Ability Tags	====================
	UE_DEFINE_GAMEPLAY_TAG(Ability_Run, "Ability.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Roll, "Ability.Roll");

	UE_DEFINE_GAMEPLAY_TAG(Ability_EquipWeapon, "Ability.EquipWeapon");
	UE_DEFINE_GAMEPLAY_TAG(Ability_UnequipWeapon, "Ability.UnequipWeapon");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Light, "Ability.Attack.Light");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_WeaponSkill, "Ability.Attack.WeaponSkill");

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

	UE_DEFINE_GAMEPLAY_TAG(Event_Combat_AttackHit, "Event.Combat.AttackHit");
	// ====================		Data Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(Data_Weapon_DefaultSword, "Data.Weapon.DefaultSword");

	UE_DEFINE_GAMEPLAY_TAG(Data_CollisionType_Weapon_Normal, "Data.CollisionType.Weapon.Normal");
	UE_DEFINE_GAMEPLAY_TAG(Data_CollisionType_Weapon_Special, "Data.CollisionType.Weapon.Special");
	UE_DEFINE_GAMEPLAY_TAG(Data_CollisionType_Body_LeftHand, "Data.CollisionType.Body.LeftHand");
	UE_DEFINE_GAMEPLAY_TAG(Data_CollisionType_Body_RightHand, "Data.CollisionType.Body.RightHand");

	UE_DEFINE_GAMEPLAY_TAG(Data_AttackType_Pierce, "Data.AttackType.Pierce");
	UE_DEFINE_GAMEPLAY_TAG(Data_AttackType_Blunt, "Data.AttackType.Blunt");
	UE_DEFINE_GAMEPLAY_TAG(Data_AttackType_Slash, "Data.AttackType.Slash");
	UE_DEFINE_GAMEPLAY_TAG(Data_AttackType_Magic, "Data.AttackType.Magic");
	UE_DEFINE_GAMEPLAY_TAG(Data_AttackScale, "Data.AttackScale");

	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Strength, "Data.Player.Stat.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Dexterity, "Data.Player.Stat.Dexterity");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Intelligence, "Data.Player.Stat.Intelligence");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_WeaponDamage, "Data.Player.Stat.WeaponDamage");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Coefficient_Strength, "Data.Player.Stat.Coefficient.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Coefficient_Dexterity, "Data.Player.Stat.Coefficient.Dexterity");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Coefficient_Intelligence, "Data.Player.Stat.Coefficient.Intelligence");

	// ====================		Character Tags	====================
}