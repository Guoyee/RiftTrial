// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RiftTrialDamageGameplayAbility.h"
#include "MeleeAttackAbility.generated.h"

// 近战攻击 GA：使用 DamageEffectClass + DamageTypes 施加伤害（无弹体）
UCLASS(Blueprintable)
class RIFTTRIAL_API UMeleeAttackAbility : public URiftTrialDamageGameplayAbility
{
    GENERATED_BODY()
};
