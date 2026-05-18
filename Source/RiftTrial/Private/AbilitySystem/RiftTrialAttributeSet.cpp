// Copyright Yerik Guo

#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "RiftTrialGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/RiftTrialAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Net/UnrealNetwork.h"
#include "Player/RiftTrialPlayerController.h"

URiftTrialAttributeSet::URiftTrialAttributeSet()
{
    InitMoveSpeed(600.f);

    const FRiftTrialGameplayTags& GameplayTags = FRiftTrialGameplayTags::Get();

    // Vital
    TagsToAttributeMap.Add(GameplayTags.Attributes_Vital_Health, GetHealthAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Vital_MaxHealth, GetMaxHealthAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Vital_Mana, GetManaAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Vital_MaxMana, GetMaxManaAttribute);

    // Offense
    TagsToAttributeMap.Add(GameplayTags.Attributes_Offense_PhysicalAttack, GetPhysicalAttackAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Offense_MagicalAttack, GetMagicalAttackAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Offense_AttackSpeed, GetAttackSpeedAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Offense_CooldownReduction, GetCooldownReductionAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Offense_CritChance, GetCritChanceAttribute);

    // Defense
    TagsToAttributeMap.Add(GameplayTags.Attributes_Defense_Armor, GetArmorAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Defense_MagicResistance, GetMagicResistanceAttribute);

    // Penetration
    TagsToAttributeMap.Add(GameplayTags.Attributes_Penetration_ArmorFlat, GetArmorPenetrationFlatAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Penetration_ArmorPercent, GetArmorPenetrationPercentAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Penetration_MagicFlat, GetMagicPenetrationFlatAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Penetration_MagicPercent, GetMagicPenetrationPercentAttribute);

    // Base
    TagsToAttributeMap.Add(GameplayTags.Attributes_Base_MoveSpeed, GetMoveSpeedAttribute);

    // Utility
    TagsToAttributeMap.Add(GameplayTags.Attributes_Utility_AttackRange, GetAttackRangeAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Utility_HealthRegeneration, GetHealthRegenerationAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Utility_ManaRegeneration, GetManaRegenerationAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Utility_Tenacity, GetTenacityAttribute);
}

void URiftTrialAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, PhysicalAttack, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, MagicalAttack, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, Armor, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, MagicResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, CooldownReduction, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, CritChance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);

    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, ArmorPenetrationFlat, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, ArmorPenetrationPercent, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, MagicPenetrationFlat, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, MagicPenetrationPercent, COND_None, REPNOTIFY_Always);

    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, AttackRange, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, Tenacity, COND_None, REPNOTIFY_Always);
}

void URiftTrialAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    }
    if (Attribute == GetManaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
    }
}

void URiftTrialAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    FEffectProperties Props;
    SetAffectProperties(Data, Props);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
    }

    if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
    }

    if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
    {
        const float LocalIncomingDamage = GetIncomingDamage();
        SetIncomingDamage(0.f);
        if (LocalIncomingDamage >= 0.f)
        {
            const float NewHealth = GetHealth() - LocalIncomingDamage;
            SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

            const bool bFatal = NewHealth <= 0.f;

            if (bFatal)
            {
                ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
                if (CombatInterface)
                {
                    CombatInterface->Die();
                }
            }
            else
            {
                FGameplayTagContainer TagContainer;
                TagContainer.AddTag(FRiftTrialGameplayTags::Get().Effects_HitReact);
                Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
            }

            const bool bBlock = URiftTrialAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
            const bool bCritical = URiftTrialAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
            ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCritical);
        }
    }
}

void URiftTrialAttributeSet::SetAffectProperties(const struct FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
    Props.EffectContextHandle = Data.EffectSpec.GetContext();
    Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

    if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
    {
        Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
        Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
        if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
        {
            if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
            {
                Props.SourceController = Pawn->GetController();
            }
        }
        if (Props.SourceController)
        {
            Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
        }
    }

    if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
    {
        Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
        Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
        Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
        Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
    }
}

void URiftTrialAttributeSet::ShowFloatingText(FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const
{
    if (ARiftTrialPlayerController* PC = Cast<ARiftTrialPlayerController>(Props.SourceController))
    {
        PC->ShowDamageNumber(Props.TargetCharacter, Damage, bBlockedHit, bCriticalHit);
    }
}

// ===== RepNotify implementations =====

#define RIFTTRIAL_REPNOTIFY(AttrName) \
    void URiftTrialAttributeSet::OnRep_##AttrName(const FGameplayAttributeData& Old) const \
    { \
        GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, AttrName, Old); \
    }

RIFTTRIAL_REPNOTIFY(Health)
RIFTTRIAL_REPNOTIFY(MaxHealth)
RIFTTRIAL_REPNOTIFY(Mana)
RIFTTRIAL_REPNOTIFY(MaxMana)
RIFTTRIAL_REPNOTIFY(PhysicalAttack)
RIFTTRIAL_REPNOTIFY(MagicalAttack)
RIFTTRIAL_REPNOTIFY(Armor)
RIFTTRIAL_REPNOTIFY(MagicResistance)
RIFTTRIAL_REPNOTIFY(AttackSpeed)
RIFTTRIAL_REPNOTIFY(CooldownReduction)
RIFTTRIAL_REPNOTIFY(CritChance)
RIFTTRIAL_REPNOTIFY(MoveSpeed)
RIFTTRIAL_REPNOTIFY(ArmorPenetrationFlat)
RIFTTRIAL_REPNOTIFY(ArmorPenetrationPercent)
RIFTTRIAL_REPNOTIFY(MagicPenetrationFlat)
RIFTTRIAL_REPNOTIFY(MagicPenetrationPercent)
RIFTTRIAL_REPNOTIFY(AttackRange)
RIFTTRIAL_REPNOTIFY(HealthRegeneration)
RIFTTRIAL_REPNOTIFY(ManaRegeneration)
RIFTTRIAL_REPNOTIFY(Tenacity)
