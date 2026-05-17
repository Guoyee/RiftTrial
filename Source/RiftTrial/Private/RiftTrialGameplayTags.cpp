// Copyright Yerik Guo

#include "RiftTrialGameplayTags.h"
#include "GameplayTagsManager.h"

FRiftTrialGameplayTags FRiftTrialGameplayTags::SingletonGameplayTags;

void FRiftTrialGameplayTags::InitializeNativeGameplayTags()
{
    /*
     * Vital
     */
    SingletonGameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Vital.Health"),
        FString("Current health")
    );
    SingletonGameplayTags.Attributes_Vital_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Vital.MaxHealth"),
        FString("Maximum health")
    );
    SingletonGameplayTags.Attributes_Vital_Mana = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Vital.Mana"),
        FString("Current mana")
    );
    SingletonGameplayTags.Attributes_Vital_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Vital.MaxMana"),
        FString("Maximum mana")
    );

    /*
     * Offense
     */
    SingletonGameplayTags.Attributes_Offense_PhysicalAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Offense.PhysicalAttack"),
        FString("Physical attack damage")
    );
    SingletonGameplayTags.Attributes_Offense_MagicalAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Offense.MagicalAttack"),
        FString("Magical ability power")
    );
    SingletonGameplayTags.Attributes_Offense_AttackSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Offense.AttackSpeed"),
        FString("Attack speed multiplier, 1.0 = default")
    );
    SingletonGameplayTags.Attributes_Offense_CooldownReduction = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Offense.CooldownReduction"),
        FString("Cooldown reduction percentage")
    );
    SingletonGameplayTags.Attributes_Offense_CritChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Offense.CritChance"),
        FString("Critical strike chance percentage")
    );

    /*
     * Defense
     */
    SingletonGameplayTags.Attributes_Defense_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Defense.Armor"),
        FString("Reduces physical damage taken")
    );
    SingletonGameplayTags.Attributes_Defense_MagicResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Defense.MagicResistance"),
        FString("Reduces magical damage taken")
    );

    /*
     * Penetration
     */
    SingletonGameplayTags.Attributes_Penetration_ArmorFlat = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Penetration.ArmorFlat"),
        FString("Flat armor penetration")
    );
    SingletonGameplayTags.Attributes_Penetration_ArmorPercent = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Penetration.ArmorPercent"),
        FString("Percent armor penetration")
    );
    SingletonGameplayTags.Attributes_Penetration_MagicFlat = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Penetration.MagicFlat"),
        FString("Flat magic penetration")
    );
    SingletonGameplayTags.Attributes_Penetration_MagicPercent = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Penetration.MagicPercent"),
        FString("Percent magic penetration")
    );

    /*
     * Base
     */
    SingletonGameplayTags.Attributes_Base_MoveSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Base.MoveSpeed"),
        FString("Movement speed")
    );

    /*
     * Utility
     */
    SingletonGameplayTags.Attributes_Utility_AttackRange = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Utility.AttackRange"),
        FString("Attack range")
    );
    SingletonGameplayTags.Attributes_Utility_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Utility.HealthRegeneration"),
        FString("Health restored per second")
    );
    SingletonGameplayTags.Attributes_Utility_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Utility.ManaRegeneration"),
        FString("Mana restored per second")
    );
    SingletonGameplayTags.Attributes_Utility_Tenacity = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Attributes.Utility.Tenacity"),
        FString("Reduces crowd control duration")
    );

    /*
     * Input Tags
     */
    SingletonGameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.LMB"),
        FString("Left Mouse Button")
    );
    SingletonGameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.RMB"),
        FString("Right Mouse Button")
    );
    SingletonGameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.1"),
        FString("Key 1")
    );
    SingletonGameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.2"),
        FString("Key 2")
    );
    SingletonGameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.3"),
        FString("Key 3")
    );
    SingletonGameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.4"),
        FString("Key 4")
    );
    SingletonGameplayTags.InputTag_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.5"),
        FString("Key 5")
    );
    SingletonGameplayTags.InputTag_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.6"),
        FString("Key 6")
    );
    SingletonGameplayTags.InputTag_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.Q"),
        FString("Key Q")
    );
    SingletonGameplayTags.InputTag_W = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.W"),
        FString("Key W")
    );
    SingletonGameplayTags.InputTag_E = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.E"),
        FString("Key E")
    );
    SingletonGameplayTags.InputTag_R = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.R"),
        FString("Key R")
    );
    SingletonGameplayTags.InputTag_A = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.A"),
        FString("Key A")
    );
    SingletonGameplayTags.InputTag_S = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.S"),
        FString("Key S")
    );
    SingletonGameplayTags.InputTag_D = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.D"),
        FString("Key D")
    );
    SingletonGameplayTags.InputTag_F = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.F"),
        FString("Key F")
    );

    /*
     * Damage Types
     */
    SingletonGameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Damage"),
        FString("Damage")
    );
    SingletonGameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Damage.Physical"),
        FString("Physical Damage Type")
    );
    SingletonGameplayTags.Damage_Magical = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Damage.Magical"),
        FString("Magical Damage Type")
    );

    /*
     * DamageType → Resistance Mappings
     */
    SingletonGameplayTags.DamageTypeToResistances.Add(
        SingletonGameplayTags.Damage_Physical,
        SingletonGameplayTags.Attributes_Defense_Armor);

    SingletonGameplayTags.DamageTypeToResistances.Add(
        SingletonGameplayTags.Damage_Magical,
        SingletonGameplayTags.Attributes_Defense_MagicResistance);

    /*
     * Effects
     */
    SingletonGameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Effects.HitReact"),
        FString("HitReact")
    );

    /*
     * Ability Tags
     */
    SingletonGameplayTags.Ability_Attack_Ranged = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Ability.Attack.Ranged"),
        FString("Ranged basic attack ability")
    );
    SingletonGameplayTags.Ability_Attack_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Ability.Attack.Melee"),
        FString("Melee basic attack ability")
    );

    /*
     * Cooldown Tags
     */
    SingletonGameplayTags.Cooldown_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Cooldown.Attack"),
        FString("Cooldown for basic attack")
    );

    /*
     * Event Tags
     */
    SingletonGameplayTags.Event_Montage_SpawnProjectile = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("Event.Montage.SpawnProjectile"),
        FString("AnimNotify event to spawn projectile during montage")
    );
}
