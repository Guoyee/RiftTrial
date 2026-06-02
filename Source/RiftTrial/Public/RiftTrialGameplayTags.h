// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * RiftTrialGameplayTags — native GameplayTags 单例
 */
struct FRiftTrialGameplayTags
{
public:
    static const FRiftTrialGameplayTags& Get() { return SingletonGameplayTags; }
    static void InitializeNativeGameplayTags();

    // ===== Vital =====
    FGameplayTag Attributes_Vital_Health;
    FGameplayTag Attributes_Vital_MaxHealth;
    FGameplayTag Attributes_Vital_Mana;
    FGameplayTag Attributes_Vital_MaxMana;

    // ===== Offense =====
    FGameplayTag Attributes_Offense_PhysicalAttack;
    FGameplayTag Attributes_Offense_MagicalAttack;
    FGameplayTag Attributes_Offense_AttackSpeed;
    FGameplayTag Attributes_Offense_CooldownReduction;
    FGameplayTag Attributes_Offense_CritChance;

    // ===== Defense =====
    FGameplayTag Attributes_Defense_Armor;
    FGameplayTag Attributes_Defense_MagicResistance;

    // ===== Penetration =====
    FGameplayTag Attributes_Penetration_ArmorFlat;
    FGameplayTag Attributes_Penetration_ArmorPercent;
    FGameplayTag Attributes_Penetration_MagicFlat;
    FGameplayTag Attributes_Penetration_MagicPercent;

    // ===== Base =====
    FGameplayTag Attributes_Base_MoveSpeed;

    // ===== Utility =====
    FGameplayTag Attributes_Utility_AttackRange;
    FGameplayTag Attributes_Utility_HealthRegeneration;
    FGameplayTag Attributes_Utility_ManaRegeneration;
    FGameplayTag Attributes_Utility_Tenacity;

    // ===== Input =====
    FGameplayTag InputTag_LMB;
    FGameplayTag InputTag_RMB;
    FGameplayTag InputTag_1;
    FGameplayTag InputTag_2;
    FGameplayTag InputTag_3;
    FGameplayTag InputTag_4;
    FGameplayTag InputTag_5;
    FGameplayTag InputTag_6;
    FGameplayTag InputTag_Q;
    FGameplayTag InputTag_W;
    FGameplayTag InputTag_E;
    FGameplayTag InputTag_R;
    FGameplayTag InputTag_A;
    FGameplayTag InputTag_S;
    FGameplayTag InputTag_D;
    FGameplayTag InputTag_F;
    FGameplayTag InputTag_Up;
    FGameplayTag InputTag_Down;
    FGameplayTag InputTag_Left;
    FGameplayTag InputTag_Right;

    // ===== Damage =====
    FGameplayTag Damage;
    FGameplayTag Damage_Physical;
    FGameplayTag Damage_Magical;
    FGameplayTag Damage_True;

    TMap<FGameplayTag, FGameplayTag> DamageTypeToResistances;

    // ===== Effects =====
    FGameplayTag Effects_HitReact;
    FGameplayTag Effects_Silence;
    FGameplayTag State_Dead;
    FGameplayTag State_Aggroed;
    FGameplayTag State_Spinning;
    FGameplayTag State_EmpoweredAttack;
    FGameplayTag State_Silenced;
    FGameplayTag State_Movement_Blocked;
    FGameplayTag State_Ability_Blocked;
    FGameplayTag Type_Minion;
    FGameplayTag Type_Hero;
    FGameplayTag Type_Tower;

    // ===== Abilities =====
    FGameplayTag Ability_Attack_Ranged;
    FGameplayTag Ability_Attack_Melee;
    FGameplayTag Cooldown_Attack;
    FGameplayTag Event_Montage_SpawnProjectile;
    FGameplayTag Event_Montage_AttackHit;
    FGameplayTag Event_Montage_Garen_SpinTick;
    FGameplayTag GameplayCue_Garen_E_Spin;
    FGameplayTag GameplayCue_Garen_Attack;
    FGameplayTag GameplayCue_Garen_Q;
    FGameplayTag GameplayCue_Garen_Q_Weapon;
    FGameplayTag GameplayCue_Garen_W;
    FGameplayTag GameplayCue_Garen_R;

    // ===== Garen 技能 =====
    FGameplayTag Ability_Garen_Q;
    FGameplayTag Cooldown_Garen_Q;
    FGameplayTag Ability_Garen_W;
    FGameplayTag Cooldown_Garen_W;
    FGameplayTag Ability_Garen_E;
    FGameplayTag Cooldown_Garen_E;
    FGameplayTag Ability_Garen_R;
    FGameplayTag Cooldown_Garen_R;

    // ===== 减伤属性 Tag =====
    FGameplayTag Attributes_Defense_DamageReduction;

    // ===== 数据传递 Tag（SetByCaller）=====
    FGameplayTag Data_CooldownDuration;
    FGameplayTag Data_BuffDuration;
    FGameplayTag Data_BuffMagnitude;

private:
    static FRiftTrialGameplayTags SingletonGameplayTags;

    FRiftTrialGameplayTags() = default;
    FRiftTrialGameplayTags(const FRiftTrialGameplayTags&) = delete;
    FRiftTrialGameplayTags& operator=(const FRiftTrialGameplayTags&) = delete;
};
