// Copyright Yerik Guo

#include "AbilitySystem/ModMagClac/MMC_GarenRDamage.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_GarenRDamage::UMMC_GarenRDamage()
{
	HealthDef.AttributeToCapture = URiftTrialAttributeSet::GetHealthAttribute();
	HealthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	HealthDef.bSnapshot = true;

	MaxHealthDef.AttributeToCapture = URiftTrialAttributeSet::GetMaxHealthAttribute();
	MaxHealthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MaxHealthDef.bSnapshot = true;

	RelevantAttributesToCapture.Add(HealthDef);
	RelevantAttributesToCapture.Add(MaxHealthDef);
}

float UMMC_GarenRDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Health = 0.f;
	GetCapturedAttributeMagnitude(HealthDef, Spec, EvaluateParameters, Health);
	Health = FMath::Max(Health, 0.f);

	float MaxHealth = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthDef, Spec, EvaluateParameters, MaxHealth);
	MaxHealth = FMath::Max(MaxHealth, 1.f);

	const float MissingHealth = MaxHealth - Health;
	const float MissingHealthRatio = 0.25f;

	// BaseDamage 按技能等级缩放: Lv1=150, Lv2=300, Lv3=450
	const float Level = Spec.GetLevel();
	const float BaseDamage = 150.f * Level;

	return BaseDamage + MissingHealth * MissingHealthRatio;
}
