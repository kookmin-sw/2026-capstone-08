// Fill out your copyright notice in the Description page of Project Settings.


#include "MortisGameplayTags.h"

namespace MortisGameplayTags
{
	// ====================		Input Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Zoom, "InputTag.Zoom");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Interact, "InputTag.Interact");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Select_Left, "InputTag.Select.Left");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Select_Right, "InputTag.Select.Right");

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
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_HitReact, "Ability.Action.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Stagger, "Ability.Action.Stagger");

	// ====================		State Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");

	UE_DEFINE_GAMEPLAY_TAG(State_Action_Attacking, "State.Action.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(State_Action_Turning, "State.Action.Turning");
	
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Idle, "State.Movement.Idle");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Strafing, "State.Movement.Strafing");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Chasing, "State.Movement.Chasing");
	
	UE_DEFINE_GAMEPLAY_TAG(State_Combat_Guarding, "State.Combat.Guarding");
	
	UE_DEFINE_GAMEPLAY_TAG(State_Enemy_Phase_1, "State.Enemy.Phase.1");
	UE_DEFINE_GAMEPLAY_TAG(State_Enemy_Phase_2, "State.Enemy.Phase.2");
	UE_DEFINE_GAMEPLAY_TAG(State_Enemy_Phase_3, "State.Enemy.Phase.3");
	// ====================		Event Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(Event_Player_SwitchWeapon, "Event.Player.SwitchWeapon");
	UE_DEFINE_GAMEPLAY_TAG(Event_Player_Complete_Roll, "Event.Player.Complete.Roll");
	UE_DEFINE_GAMEPLAY_TAG(Event_Player_Success_LightAttack, "Event.Player.Success.LightAttack");
	UE_DEFINE_GAMEPLAY_TAG(Event_Player_Success_SkillAttack, "Event.Player.Success.SkillAttack");

	UE_DEFINE_GAMEPLAY_TAG(Event_RuneSet_Slash_Exaltation_Done, "Event.RuneSet.Slash.Exaltation.Done");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Action_ExecutePattern, "Event.Action.ExecutePattern");
	UE_DEFINE_GAMEPLAY_TAG(Event_Action_Turn, "Event.Action.Turn");
	UE_DEFINE_GAMEPLAY_TAG(Event_Action_HitReact, "Event.Action.HitReact");

	UE_DEFINE_GAMEPLAY_TAG(Event_Combat_AttackHit, "Event.Combat.AttackHit");
	UE_DEFINE_GAMEPLAY_TAG(Event_Combat_Combo_Next, "Event.Combat.Combo.Next");
	UE_DEFINE_GAMEPLAY_TAG(Event_Combat_Attack_Blocked, "Event.Combat.Attack.Blocked");
	UE_DEFINE_GAMEPLAY_TAG(Event_Combat_Block_Success, "Event.Combat.Block.Success");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Montage_EndWarpUpdate, "Event.Montage.EndWarpUpdate");

	// ====================		Data Tags		====================
	UE_DEFINE_GAMEPLAY_TAG(Data_Weapon_DefaultSword, "Data.Weapon.DefaultSword");
	UE_DEFINE_GAMEPLAY_TAG(Data_Weapon_FallenKnightSword, "Data.Weapon.FallenKnightSword");
	UE_DEFINE_GAMEPLAY_TAG(Data_Shield, "Data.Shield");

	UE_DEFINE_GAMEPLAY_TAG(Data_CollisionType_Weapon_Normal, "Data.CollisionType.Weapon.Normal");
	UE_DEFINE_GAMEPLAY_TAG(Data_CollisionType_Weapon_Special, "Data.CollisionType.Weapon.Special");
	UE_DEFINE_GAMEPLAY_TAG(Data_CollisionType_Shield, "Data.CollisionType.Shield");

	UE_DEFINE_GAMEPLAY_TAG(Data_AttackType_Pierce, "Data.AttackType.Pierce");
	UE_DEFINE_GAMEPLAY_TAG(Data_AttackType_Blunt, "Data.AttackType.Blunt");
	UE_DEFINE_GAMEPLAY_TAG(Data_AttackType_Slash, "Data.AttackType.Slash");
	UE_DEFINE_GAMEPLAY_TAG(Data_AttackType_Magic, "Data.AttackType.Magic");
	UE_DEFINE_GAMEPLAY_TAG(Data_AttackScale, "Data.AttackScale");
	
	UE_DEFINE_GAMEPLAY_TAG(Data_Stat_PoiseDamage, "Data.Stat.PoiseDamage");

	UE_DEFINE_GAMEPLAY_TAG(Data_RuneValue, "Data.RuneValue");
	UE_DEFINE_GAMEPLAY_TAG(Data_Cost_Health, "Data.Cost.Health");
	UE_DEFINE_GAMEPLAY_TAG(Data_Cost_Stamina, "Data.Cost.Stamina");
	UE_DEFINE_GAMEPLAY_TAG(Data_Cost_Mana, "Data.Cost.Mana");

	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_MaxHP, "Data.Player.Stat.MaxHP");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_MaxStamina, "Data.Player.Stat.MaxStamina");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_StaminaReductionRate, "Data.Player.Stat.StaminaReductionRate");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_StaminaRegen, "Data.Player.Stat.StaminaRegen");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_MaxMana, "Data.Player.Stat.MaxMana");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_ManaRegen, "Data.Player.Stat.ManaRegen");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_ManaReductionRate, "Data.Player.Stat.ManaReductionRate");

	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Strength, "Data.Player.Stat.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Dexterity, "Data.Player.Stat.Dexterity");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Intelligence, "Data.Player.Stat.Intelligence");

	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_WeaponDamage, "Data.Player.Stat.WeaponDamage");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Coefficient_Strength, "Data.Player.Stat.Coefficient.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Coefficient_Dexterity, "Data.Player.Stat.Coefficient.Dexterity");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Coefficient_Intelligence, "Data.Player.Stat.Coefficient.Intelligence");
	

	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_BonusDamage_Percent_All, "Data.Player.Stat.BonusDamage.Percent.All");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_BonusDamage_Percent_Pierce, "Data.Player.Stat.BonusDamage.Percent.Pierce");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_BonusDamage_Percent_Blunt, "Data.Player.Stat.BonusDamage.Percent.Blunt");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_BonusDamage_Percent_Slash, "Data.Player.Stat.BonusDamage.Percent.Slash");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_BonusDamage_Percent_Magic, "Data.Player.Stat.BonusDamage.Percent.Magic");

	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Speed_LightAttack, "Data.Player.Stat.Speed.LightAttack");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Speed_WeaponSkill, "Data.Player.Stat.Speed.WeaponSkill");
	UE_DEFINE_GAMEPLAY_TAG(Data_Player_Stat_Speed_Move, "Data.Player.Stat.Speed.Move");

	UE_DEFINE_GAMEPLAY_TAG(Data_Enemy_Stat_WeaponDamage, "Data.Enemy.Stat.WeaponDamage");
	
	// ====================		RuneSet Tags	====================
	UE_DEFINE_GAMEPLAY_TAG(Rune_Set_Slashing, "Rune.Set.Slashing");
	UE_DEFINE_GAMEPLAY_TAG(Rune_Set_Piercing, "Rune.Set.Piercing");
	UE_DEFINE_GAMEPLAY_TAG(Rune_Set_Combo, "Rune.Set.Combo");
	UE_DEFINE_GAMEPLAY_TAG(Rune_Set_Mystery, "Rune.Set.Mystery");
	UE_DEFINE_GAMEPLAY_TAG(Rune_Set_Inspiration, "Rune.Set.Inspiration");

	// ====================		Character Tags	====================
}