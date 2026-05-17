// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "RiftTrialAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class RIFTTRIAL_API URiftTrialAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
    //在调用MakeEffectContext()创建Handle后，该指针的生命周期由Handle接管
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
