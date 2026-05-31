// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RiftTrialGameplayAbility.h"
#include "RiftTrialDamageGameplayAbility.generated.h"

// 伤害类 GA 基类：提供黑板目标读取、攻速冷却计算、连击动画轮换
UCLASS(Blueprintable)
class RIFTTRIAL_API URiftTrialDamageGameplayAbility : public URiftTrialGameplayAbility
{
    GENERATED_BODY()

public:
    // 获取当前攻击目标：优先从 AI 黑板读取（小兵/塔），
    // 若无效则从 ARiftTrialCharacter::CurrentAttackTarget 读取（玩家英雄）
    UFUNCTION(BlueprintCallable, Category = "Attack")
    virtual AActor* GetAttackTarget();

    // 攻击蒙太奇动画列表，按顺序轮换播放
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RiftTrial|Ability")
    TArray<TObjectPtr<UAnimMontage>> AttackMontages;

    // 获取下一段攻击蒙太奇（按顺序轮换）
    UFUNCTION(BlueprintCallable, Category = "RiftTrial|Ability")
    UAnimMontage* GetNextAttackMontage();

    // 获取当前攻速（用于蒙太奇播放速度），默认 1.0
    UFUNCTION(BlueprintCallable, Category = "RiftTrial|Ability")
    float GetAttackSpeed() const;

    // 向目标直接施加伤害 GE（使用 DamageEffectClass + DamageTypes）
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void ApplyDamageToTarget(AActor* Target);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RiftTrial|Attribute")
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Ability")
    TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
