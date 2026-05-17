// Copyright Yerik Guo

#include "AbilitySystem/ModMagClac/MMC_MaxHealth.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
    check(CombatInterface);
    const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

    return 80.f + 80.f * PlayerLevel;
}
