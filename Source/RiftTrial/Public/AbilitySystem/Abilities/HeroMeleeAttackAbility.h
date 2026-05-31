// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HeroGameplayAbility.h"
#include "HeroMeleeAttackAbility.generated.h"

// 英雄近战普攻 GA（单次激活 / One-shot）
// ─────────────────────────────────────────
// 前摇：蒙太奇播放 → 可被移动取消 → 不触发冷却
// 命中：AnimNotify 触发 → 施加伤害 + CommitAbility（冷却开始）
// 后摇：剩余动画继续播放，冷却并行流逝
// 重激活：Character::Tick 在冷却结束 + 目标有效时自动 TryActivate
//         效果等同于"输入缓冲"
// ─────────────────────────────────────────
// 蒙太奇设置：BlendOut = 0, BlendIn = 0.1, TickType = BranchingPoint
UCLASS(Blueprintable)
class RIFTTRIAL_API UHeroMeleeAttackAbility : public UHeroGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    // 命中帧回调：施加伤害 + 提交冷却
    UFUNCTION()
    void OnAttackHitReceived(FGameplayEventData Payload);

    // 蒙太奇正常播完 → 结束 GA
    UFUNCTION()
    void OnMontageCompleted();

    // 蒙太奇被打断（玩家移动取消前摇）→ 结束 GA，不触发冷却
    UFUNCTION()
    void OnMontageInterrupted();

    // Q 强化普攻专属蒙太奇（替换普通 AttackMontages）
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Q")
    TObjectPtr<UAnimMontage> QAttackMontage;

    // Q 强化普攻：额外伤害 GE
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Q")
    TSubclassOf<UGameplayEffect> QBonusDamageEffectClass;

    // Q 强化普攻：沉默 DEBUFF GE
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Q")
    TSubclassOf<UGameplayEffect> QSilenceEffectClass;

    // 施加 Q 额外伤害 + 沉默到目标
    void ApplyQEmpoweredAttack(AActor* Target);
};
