// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RiftTrialGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class RIFTTRIAL_API URiftTrialGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()
    
public:
    
    UPROPERTY(EditDefaultsOnly, Category="Input")
    //该技能的默认InputTag(默认键位), 在蓝图的details中设置 
    FGameplayTag StartupInputTag;
    
};
