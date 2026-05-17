// Copyright Yerik Guo

#include "AbilitySystem/ModMagClac/MMC_MaxMana.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
    check(CombatInterface);
    const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

    return 50.f + 40.f * PlayerLevel;
}
