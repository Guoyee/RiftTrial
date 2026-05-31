// Copyright Yerik Guo

#include "AbilitySystem/ModMagClac/MMC_BuffDuration.h"
#include "RiftTrialGameplayTags.h"

float UMMC_BuffDuration::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	return Spec.GetSetByCallerMagnitude(
		FRiftTrialGameplayTags::Get().Data_BuffDuration, false, 0.f);
}
