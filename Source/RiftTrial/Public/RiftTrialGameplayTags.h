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

    TMap<FGameplayTag, FGameplayTag> DamageTypeToResistances;

    // ===== Effects =====
    FGameplayTag Effects_HitReact;

    // ===== Abilities =====
    FGameplayTag Ability_Attack_Ranged;
    FGameplayTag Ability_Attack_Melee;
    FGameplayTag Cooldown_Attack;
    FGameplayTag Event_Montage_SpawnProjectile;
    FGameplayTag Event_Montage_AttackHit;

private:
    static FRiftTrialGameplayTags SingletonGameplayTags;

    FRiftTrialGameplayTags() = default;
    FRiftTrialGameplayTags(const FRiftTrialGameplayTags&) = delete;
    FRiftTrialGameplayTags& operator=(const FRiftTrialGameplayTags&) = delete;
};
