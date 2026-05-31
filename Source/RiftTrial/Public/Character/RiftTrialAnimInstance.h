// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RiftTrialAnimInstance.generated.h"

class UAbilitySystemComponent;

// 英雄动画实例：缓存 GAS 标签状态，供 AnimBP 驱动状态机
UCLASS()
class RIFTTRIAL_API URiftTrialAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    // Q 强化普攻状态 → AnimBP 切换 Q 专属移动动画
    UPROPERTY(BlueprintReadOnly, Category = "RiftTrial|Anim")
    bool bIsQEmpowered = false;

private:
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> CachedASC;
};
