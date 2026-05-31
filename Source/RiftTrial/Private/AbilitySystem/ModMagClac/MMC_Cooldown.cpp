// Copyright Yerik Guo

#include "AbilitySystem/ModMagClac/MMC_Cooldown.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "RiftTrialGameplayTags.h"

UMMC_Cooldown::UMMC_Cooldown()
{
	CDRDef.AttributeToCapture = URiftTrialAttributeSet::GetCooldownReductionAttribute();
	CDRDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	CDRDef.bSnapshot = true;

	RelevantAttributesToCapture.Add(CDRDef);
}

float UMMC_Cooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 基础冷却（由 GA 通过 SetByCaller 传入）
	float BaseDuration = Spec.GetSetByCallerMagnitude(
		FRiftTrialGameplayTags::Get().Data_CooldownDuration, false, 0.f);

	// 冷却缩减
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;

	float CDR = 0.f;
	GetCapturedAttributeMagnitude(CDRDef, Spec, EvaluateParameters, CDR);
	CDR = FMath::Clamp(CDR, 0.f, 0.45f); // LoL 上限 45%

	return BaseDuration * (1.f - CDR);
}
