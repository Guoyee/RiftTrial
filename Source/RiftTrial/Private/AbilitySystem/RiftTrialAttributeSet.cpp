// Copyright Yerik Guo


#include "AbilitySystem/RiftTrialAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "RiftTrialGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/RiftTrialAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/RiftTrialPlayerController.h"

URiftTrialAttributeSet::URiftTrialAttributeSet()
{
    const FRiftTrialGameplayTags& GameplayTags = FRiftTrialGameplayTags::Get();
    
    //Primary Attributes
    TagsToAttributeMap.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);
    
    // Secondary Attributes
    TagsToAttributeMap.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Resistance_Fire, GetResistanceFireAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Resistance_Lightning, GetResistanceLightningAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Resistance_Arcane, GetResistanceArcaneAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Resistance_Physical, GetResistancePhysicalAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Secondary_AttackSpeed, GetAttackSpeedAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
    TagsToAttributeMap.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
}

void URiftTrialAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    //Primary Attributes
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, Strength, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

    // Secondary Attributes
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, Armor, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, ResistanceFire, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, ResistanceLightning, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, ResistanceArcane, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, ResistancePhysical, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    
    //Vital Attributes
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URiftTrialAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    

}

void URiftTrialAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
        //UE_LOG(LogTemp, Warning, TEXT("Health %f"), NewValue);
    }
    if (Attribute == GetManaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
        //UE_LOG(LogTemp, Warning, TEXT("Mana %f"), NewValue);
    }
    
}

void URiftTrialAttributeSet::SetAffectProperties(const struct FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
    //Source = cause of the effect, Target = target of the effect(owner of this AS)
    
    Props.EffectContextHandle = Data.EffectSpec.GetContext();
    Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
    
    if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid()&&Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
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


void URiftTrialAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    
    FEffectProperties Props;
    SetAffectProperties(Data, Props);
    
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
        UE_LOG(LogTemp, Warning, TEXT("Changed Health on %s, Health: %f"), *Props.TargetAvatarActor->GetName(), GetHealth());
    }
    
    if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
    }
    
    //处理Damage meta Attributes
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
                //死亡逻辑
                ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
                if (CombatInterface)
                {
                    CombatInterface->Die();
                }
            }
            else
            {
                //受击逻辑
                UE_LOG(LogTemp, Warning, TEXT("HitReact activated on %s"), *Props.TargetAvatarActor->GetName());
                FGameplayTagContainer TagContainer;
                TagContainer.AddTag(FRiftTrialGameplayTags::Get().Effects_HitReact);
                Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
            }
            
            //伤害跳字
            const bool bBlock = URiftTrialAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
            const bool bCritical = URiftTrialAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);

            ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCritical);
        }
    }
}

void URiftTrialAttributeSet::ShowFloatingText(FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const
{
    if (ARiftTrialPlayerController* AuraPC = Cast<ARiftTrialPlayerController>(Props.SourceController))
    {
        AuraPC->ShowDamageNumber(Props.TargetCharacter, Damage, bBlockedHit, bCriticalHit);
    }
}

void URiftTrialAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, Strength, OldStrength);
}

void URiftTrialAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, Intelligence, OldIntelligence);
}

void URiftTrialAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, Resilience, OldResilience);
}

void URiftTrialAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, Vigor, OldVigor);
}

void URiftTrialAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, Health, OldHealth);
}

void URiftTrialAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, MaxHealth, OldMaxHealth);
}

void URiftTrialAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, Mana, OldMana);
}

void URiftTrialAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, MaxMana, OldMaxMana);
}

// Secondary Attributes RepNotify 实现
void URiftTrialAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, Armor, OldArmor);
}

void URiftTrialAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void URiftTrialAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, BlockChance, OldBlockChance);
}

void URiftTrialAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void URiftTrialAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void URiftTrialAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void URiftTrialAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void URiftTrialAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void URiftTrialAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, AttackSpeed, OldAttackSpeed);
}

void URiftTrialAttributeSet::OnRep_ResistanceFire(const FGameplayAttributeData& OldResistanceFire) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, ResistanceFire, OldResistanceFire);
}

void URiftTrialAttributeSet::OnRep_ResistanceLightning(const FGameplayAttributeData& OldResistanceLightning) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, ResistanceLightning, OldResistanceLightning);
}

void URiftTrialAttributeSet::OnRep_ResistanceArcane(const FGameplayAttributeData& OldResistanceArcane) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, ResistanceArcane, OldResistanceArcane);
}

void URiftTrialAttributeSet::OnRep_ResistancePhysical(const FGameplayAttributeData& OldResistancePhysical) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URiftTrialAttributeSet, ResistancePhysical, OldResistancePhysical);
}

