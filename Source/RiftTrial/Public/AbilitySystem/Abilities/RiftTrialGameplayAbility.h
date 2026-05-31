// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RiftTrialGameplayAbility.generated.h"

// 全局技能基类：输入绑定 + 取消防护 + 激活时间戳
UCLASS()
class RIFTTRIAL_API URiftTrialGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;

    virtual void InputPressed(const FGameplayAbilitySpecHandle Handle,
                              const FGameplayAbilityActorInfo* ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo) override;

    // 默认键位
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Input")
    FGameplayTag StartupInputTag;

    // 激活时再次按键 → 取消技能
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Input")
    bool bCancelOnRePress = false;

    // 技能激活时间戳，防误触（GetWorld()->GetTimeSeconds()）
    float ActivateTime = 0.f;
};
