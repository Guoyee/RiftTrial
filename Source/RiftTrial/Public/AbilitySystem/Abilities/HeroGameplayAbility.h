// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RiftTrialGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "HeroGameplayAbility.generated.h"

class UGameplayEffect;
class UAnimMontage;

// 自身 Buff 定义：GE 类 + 动态参数
USTRUCT(BlueprintType)
struct FSelfBuffDef
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Buff")
    TSubclassOf<UGameplayEffect> EffectClass;

    // 持续时间（秒），>0 时通过 SetByCaller(Data.BuffDuration) 传入
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Buff")
    float Duration = 0.f;

    // 数值（SetByCaller Data.BuffMagnitude），GE Modifier 用 SetByCaller 接收
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Buff")
    float Magnitude = 0.f;

    // 可选：动态注入到 GrantedTags
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Buff")
    FGameplayTag GrantedTag;
};

// 英雄技能基类：冷却覆盖 + 自身 Buff + 伤害工具
// 小兵 / 塔走 URiftTrialDamageGameplayAbility 分支，不继承此类
UCLASS()
class RIFTTRIAL_API UHeroGameplayAbility : public URiftTrialGameplayAbility
{
    GENERATED_BODY()

public:
    // ── 冷却 ──
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Cooldown")
    FGameplayTag CooldownTag;

    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Cooldown")
    float BaseCooldownDuration = 1.f;

    // ── 伤害 ──
    UFUNCTION(BlueprintCallable, Category = "RiftTrial|Ability")
    virtual AActor* GetAttackTarget();

    UFUNCTION(BlueprintCallable, Category = "RiftTrial|Ability")
    float GetAttackSpeed() const;

    UFUNCTION(BlueprintCallable, Category = "RiftTrial|Ability")
    void ApplyDamageToTarget(AActor* Target);

    // ── 攻击蒙太奇 ──
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RiftTrial|Ability")
    TArray<TObjectPtr<UAnimMontage>> AttackMontages;

    UFUNCTION(BlueprintCallable, Category = "RiftTrial|Ability")
    UAnimMontage* GetNextAttackMontage();

    // 获取鼠标悬停目标（指向性技能如 R 使用）
    UFUNCTION(BlueprintCallable, Category = "RiftTrial|Ability")
    AActor* GetCursorTarget() const;

protected:
    // ── 自身 Buff ──
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Buff")
    TArray<FSelfBuffDef> SelfBuffs;

    void ApplySelfBuffs();

    // ── 伤害配置 ──
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RiftTrial|Attribute")
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Ability")
    TMap<FGameplayTag, FScalableFloat> DamageTypes;

    // ── 冷却覆盖 ──
    virtual const FGameplayTagContainer* GetCooldownTags() const override;
    virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                               const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo) const override;

private:
    mutable FGameplayTagContainer CachedCooldownTags;
};
