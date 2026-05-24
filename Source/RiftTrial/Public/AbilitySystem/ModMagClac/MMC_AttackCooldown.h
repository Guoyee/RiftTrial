// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_AttackCooldown.generated.h"

// 根据 AttackSpeed 计算攻击冷却时长：1.0 / AttackSpeed
UCLASS()
class RIFTTRIAL_API UMMC_AttackCooldown : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    UMMC_AttackCooldown();

    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
    FGameplayEffectAttributeCaptureDefinition AttackSpeedDef;
};
