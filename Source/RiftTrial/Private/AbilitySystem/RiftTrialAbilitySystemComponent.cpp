// Copyright Yerik Guo


#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/RiftTrialGameplayAbility.h"
#include "RiftTrialGameplayTags.h"

void URiftTrialAbilitySystemComponent::AbilityActorInfoSet()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &URiftTrialAbilitySystemComponent::ClientEffectApplied);
    
}

void URiftTrialAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
    for (const TSubclassOf<UGameplayAbility> Ability : Abilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
        //对于玩家技能，绑定一个默认快捷键
        if (const URiftTrialGameplayAbility* RiftTrialGameplayAbility = Cast<URiftTrialGameplayAbility>(AbilitySpec.Ability))
        {
            AbilitySpec.DynamicAbilityTags.AddTag(RiftTrialGameplayAbility->StartupInputTag);
        }
        GiveAbility(AbilitySpec);
    }    
}

void URiftTrialAbilitySystemComponent::AbilityInputTagHeld(FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;
    
    for (FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
    {
        if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
        {
            AbilitySpecInputPressed(AbilitySpec);
            if (!AbilitySpec.IsActive())
            {
                TryActivateAbility(AbilitySpec.Handle);
            }
        }
    }
}

void URiftTrialAbilitySystemComponent::AbilityInputTagReleased(FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;
    
    for (FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
    {
        if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
        {
            AbilitySpecInputReleased(AbilitySpec);
        }
    }
}

void URiftTrialAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle) const
{
    FGameplayTagContainer TagContainer;
    EffectSpec.GetAllAssetTags(TagContainer);
    
    EffectAssetTags.Broadcast(TagContainer);
    
}
