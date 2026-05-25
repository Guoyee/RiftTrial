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
    // 从黑板 "TargetToFollow" 获取当前攻击目标
    UFUNCTION(BlueprintCallable, Category = "Attack")
    AActor* GetAttackTarget();

    // 攻击蒙太奇动画列表，按顺序轮换播放
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CAT_ABILITY)
    TArray<TObjectPtr<UAnimMontage>> AttackMontages;

    // 获取下一段攻击蒙太奇（按顺序轮换）
    UFUNCTION(BlueprintCallable, Category = CAT_ABILITY)
    UAnimMontage* GetNextAttackMontage();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CAT_ATTRIBUTE)
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, Category = CAT_ABILITY)
    TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
