// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RiftTrialDamageGameplayAbility.h"
#include "MeleeAttackAbility.generated.h"

// 近战攻击 GA：直接向目标施加伤害 GE，无弹体
UCLASS(Blueprintable)
class RIFTTRIAL_API UMeleeAttackAbility : public URiftTrialDamageGameplayAbility
{
    GENERATED_BODY()

public:
    // 向目标直接施加伤害 GE（无弹体）
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void ApplyDamageToTarget(AActor* Target);
};
