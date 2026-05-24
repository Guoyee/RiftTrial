// Copyright Yerik Guo

#include "AbilitySystem/ModMagClac/MMC_AttackCooldown.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"

UMMC_AttackCooldown::UMMC_AttackCooldown()
{
    AttackSpeedDef.AttributeToCapture = URiftTrialAttributeSet::GetAttackSpeedAttribute();
    AttackSpeedDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
    AttackSpeedDef.bSnapshot = false;

    RelevantAttributesToCapture.Add(AttackSpeedDef);
}

float UMMC_AttackCooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;

    float AttackSpeed = 0.f;
    GetCapturedAttributeMagnitude(AttackSpeedDef, Spec, EvaluateParameters, AttackSpeed);
    AttackSpeed = FMath::Max(AttackSpeed, 0.01f);

    return 1.0f / AttackSpeed;
}
