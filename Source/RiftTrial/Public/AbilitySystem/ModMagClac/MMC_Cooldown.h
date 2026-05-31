// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Cooldown.generated.h"

// 通用冷却计算：BaseCooldown * (1 - CooldownReduction)
// BaseCooldown 由 GA 通过 SetByCaller(Data.CooldownDuration) 传入
UCLASS()
class RIFTTRIAL_API UMMC_Cooldown : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    UMMC_Cooldown();

    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
    FGameplayEffectAttributeCaptureDefinition CDRDef;
};
