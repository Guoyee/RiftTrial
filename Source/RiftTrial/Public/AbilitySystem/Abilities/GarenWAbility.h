// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HeroGameplayAbility.h"
#include "GarenWAbility.generated.h"

// 盖伦 W — 勇气
// 激活时应用 SelfBuffs 列表中的 GE（减伤 + 韧性）
UCLASS(Blueprintable)
class RIFTTRIAL_API UGarenWAbility : public UHeroGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;
};
