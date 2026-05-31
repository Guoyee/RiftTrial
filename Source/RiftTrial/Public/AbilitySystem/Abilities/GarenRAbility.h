// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HeroGameplayAbility.h"
#include "GarenRAbility.generated.h"

// 盖伦 R — 德玛西亚正义 (Demacian Justice)
// 指向性斩杀：对当前目标造成真实伤害（基于已损失生命值）
// CancelAbilitiesWithTag = State.Spinning（激活时打断 E）
UCLASS(Blueprintable)
class RIFTTRIAL_API UGarenRAbility : public UHeroGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                    const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayTagContainer* SourceTags,
                                    const FGameplayTagContainer* TargetTags,
                                    FGameplayTagContainer* OptionalRelevantTags) const override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    // 命中帧 → 施加斩杀伤害
    UFUNCTION()
    void OnRHitReceived(FGameplayEventData Payload);

    // 蒙太奇完成
    UFUNCTION()
    void OnRMontageCompleted();

    // 大招蒙太奇
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|R")
    TObjectPtr<UAnimMontage> RMontage;

    // 施法距离
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|R")
    float CastRange = 600.f;

    // 激活时锁定的目标
    UPROPERTY()
    TObjectPtr<AActor> RTarget;
};
