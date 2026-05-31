// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_BuffDuration.generated.h"

// 读取 SetByCaller Data.BuffDuration，直接作为 GE 的 Duration
UCLASS()
class RIFTTRIAL_API UMMC_BuffDuration : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
