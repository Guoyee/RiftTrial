// Copyright Yerik Guo

#include "AbilitySystem/Abilities/RiftTrialGameplayAbility.h"

void URiftTrialGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilityActivationInfo ActivationInfo,
                                                const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UWorld* World = GetWorld())
	{
		ActivateTime = World->GetTimeSeconds();
	}
}

void URiftTrialGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (!bCancelOnRePress) return;

	const float TimeSinceActivate = GetWorld()->GetTimeSeconds() - ActivateTime;
	if (TimeSinceActivate < 0.2f) return;

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
