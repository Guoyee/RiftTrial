// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "RiftTrialAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
    GENERATED_BODY()

    FEffectProperties(){}

    FGameplayEffectContextHandle EffectContextHandle;

    UPROPERTY()
    UAbilitySystemComponent* SourceASC = nullptr;

    UPROPERTY()
    AActor* SourceAvatarActor = nullptr;

    UPROPERTY()
    AController* SourceController = nullptr;

    UPROPERTY()
    ACharacter* SourceCharacter = nullptr;

    UPROPERTY()
    UAbilitySystemComponent* TargetASC = nullptr;

    UPROPERTY()
    AActor* TargetAvatarActor = nullptr;

    UPROPERTY()
    AController* TargetController = nullptr;

    UPROPERTY()
    ACharacter* TargetCharacter = nullptr;
};

template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

UCLASS()
class RIFTTRIAL_API URiftTrialAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    URiftTrialAttributeSet();
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

    TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributeMap;

    // ===== Vital =====
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, Health);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, MaxHealth);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital")
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, Mana);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital")
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, MaxMana);

    // ===== Offense =====
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalAttack, Category = "Offense")
    FGameplayAttributeData PhysicalAttack;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, PhysicalAttack);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicalAttack, Category = "Offense")
    FGameplayAttributeData MagicalAttack;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, MagicalAttack);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Defense")
    FGameplayAttributeData Armor;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, Armor);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicResistance, Category = "Defense")
    FGameplayAttributeData MagicResistance;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, MagicResistance);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "Offense")
    FGameplayAttributeData AttackSpeed;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, AttackSpeed);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CooldownReduction, Category = "Offense")
    FGameplayAttributeData CooldownReduction;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, CooldownReduction);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritChance, Category = "Offense")
    FGameplayAttributeData CritChance;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, CritChance);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "Base")
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, MoveSpeed);

    // ===== Penetration =====
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetrationFlat, Category = "Penetration")
    FGameplayAttributeData ArmorPenetrationFlat;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, ArmorPenetrationFlat);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetrationPercent, Category = "Penetration")
    FGameplayAttributeData ArmorPenetrationPercent;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, ArmorPenetrationPercent);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicPenetrationFlat, Category = "Penetration")
    FGameplayAttributeData MagicPenetrationFlat;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, MagicPenetrationFlat);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicPenetrationPercent, Category = "Penetration")
    FGameplayAttributeData MagicPenetrationPercent;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, MagicPenetrationPercent);

    // ===== Utility =====
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackRange, Category = "Utility")
    FGameplayAttributeData AttackRange;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, AttackRange);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Utility")
    FGameplayAttributeData HealthRegeneration;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, HealthRegeneration);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegeneration, Category = "Utility")
    FGameplayAttributeData ManaRegeneration;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, ManaRegeneration);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Tenacity, Category = "Utility")
    FGameplayAttributeData Tenacity;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, Tenacity);

    // W等减伤技能使用 (0.0~1.0)
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageReduction, Category = "Defense")
    FGameplayAttributeData DamageReduction;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, DamageReduction);

    // ===== Meta (transient, not replicated) =====
    UPROPERTY(BlueprintReadOnly, Category = "Meta")
    FGameplayAttributeData IncomingDamage;
    ATTRIBUTE_ACCESSORS(URiftTrialAttributeSet, IncomingDamage);

    // ===== RepNotify =====
    UFUNCTION() void OnRep_Health(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_PhysicalAttack(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_MagicalAttack(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_Armor(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_MagicResistance(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_AttackSpeed(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_CooldownReduction(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_CritChance(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_MoveSpeed(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_ArmorPenetrationFlat(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_ArmorPenetrationPercent(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_MagicPenetrationFlat(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_MagicPenetrationPercent(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_AttackRange(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_HealthRegeneration(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_ManaRegeneration(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_Tenacity(const FGameplayAttributeData& Old) const;
    UFUNCTION() void OnRep_DamageReduction(const FGameplayAttributeData& Old) const;

private:
    void SetAffectProperties(const struct FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
    void ShowFloatingText(FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const;
};
