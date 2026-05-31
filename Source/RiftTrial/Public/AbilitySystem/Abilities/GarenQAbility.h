// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HeroGameplayAbility.h"
#include "GarenQAbility.generated.h"

// 盖伦 Q — 致命打击
// 激活时应用 SelfBuffs 列表中的 GE（移速 + 强化普攻标签）
UCLASS(Blueprintable)
class RIFTTRIAL_API UGarenQAbility : public UHeroGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;
};
