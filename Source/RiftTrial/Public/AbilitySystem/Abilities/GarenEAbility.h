// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HeroGameplayAbility.h"
#include "GarenEAbility.generated.h"

// 盖伦 E — 审判 (Judgment)
// ─────────────────────────────
// 旋转 3s，每 0.5s 对周围敌人造成物理伤害
// 期间 State.Spinning 阻塞普攻，Q/W 可正常使用，再按 E 取消
UCLASS(Blueprintable)
class RIFTTRIAL_API UGarenEAbility : public UHeroGameplayAbility
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
    // 伤害计时器回调
    void OnSpinTick();

    // AoE 检测并造成伤害
    void DealSpinDamage();


    // 总持续时间
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|E")
    float SpinDuration = 3.f;

    // 每次伤害间隔（随攻速缩放）
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|E")
    float TickInterval = 0.5f;

    // AoE 半径
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|E")
    float SpinRadius = 300.f;

    // 旋转蒙太奇（Looping）
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|E")
    TObjectPtr<UAnimMontage> SpinMontage;

private:
    FTimerHandle SpinTimerHandle;
    FTimerHandle DurationTimerHandle;
};
