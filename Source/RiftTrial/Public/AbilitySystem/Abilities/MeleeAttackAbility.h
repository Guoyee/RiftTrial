// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RiftTrialDamageGameplayAbility.h"
#include "MeleeAttackAbility.generated.h"

// 近战攻击 GA：从黑板读取目标，直接向目标施加伤害 GE，无弹体
UCLASS(Blueprintable)
class RIFTTRIAL_API UMeleeAttackAbility : public URiftTrialDamageGameplayAbility
{
    GENERATED_BODY()

public:
    // 从黑板 "TargetToFollow" 获取当前攻击目标
    UFUNCTION(BlueprintCallable, Category = "Attack")
    AActor* GetAttackTarget();

    // 向目标直接施加伤害 GE（无弹体）
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void ApplyDamageToTarget(AActor* Target);

    // 攻击蒙太奇动画列表，按顺序轮换播放
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
    TArray<TObjectPtr<UAnimMontage>> AttackMontages;

    // 获取下一段攻击蒙太奇（按顺序轮换）
    UFUNCTION(BlueprintCallable, Category = "Montage")
    UAnimMontage* GetNextAttackMontage();

    // 根据 AttackSpeed 属性计算冷却时长：CD = 1.0 / AttackSpeed
    UFUNCTION(BlueprintCallable, Category = "Cooldown")
    void ApplyAttackCooldown();
};
