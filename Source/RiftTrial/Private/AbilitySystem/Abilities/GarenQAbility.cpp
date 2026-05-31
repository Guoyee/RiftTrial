// Copyright Yerik Guo

#include "AbilitySystem/Abilities/GarenQAbility.h"
#include "AbilitySystemComponent.h"
#include "RiftTrialGameplayTags.h"

void UGarenQAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ApplySelfBuffs();

	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		ASC->ExecuteGameplayCue(FRiftTrialGameplayTags::Get().GameplayCue_Garen_Q);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
