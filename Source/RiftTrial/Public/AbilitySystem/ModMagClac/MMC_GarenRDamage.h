// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_GarenRDamage.generated.h"

// 盖伦 R 伤害计算：BaseDamage + MissingHealth * Ratio
// MissingHealth = MaxHealth - CurrentHealth
UCLASS()
class RIFTTRIAL_API UMMC_GarenRDamage : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    UMMC_GarenRDamage();

    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
    FGameplayEffectAttributeCaptureDefinition HealthDef;
    FGameplayEffectAttributeCaptureDefinition MaxHealthDef;
};
