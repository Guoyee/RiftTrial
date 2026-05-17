// Copyright Yerik Guo


#include "AbilitySystem/RiftTrialAbilitySystemGlobals.h"

#include "RiftTrialAbilityTypes.h"

FGameplayEffectContext* URiftTrialAbilitySystemGlobals::AllocGameplayEffectContext() const
{
    return new FRiftTrialGameplayEffectContext();
}
