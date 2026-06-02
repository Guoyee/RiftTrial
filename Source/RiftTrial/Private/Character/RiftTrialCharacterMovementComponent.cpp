// Copyright Yerik Guo

#include "Character/RiftTrialCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "RiftTrialGameplayTags.h"

float URiftTrialCharacterMovementComponent::GetMaxSpeed() const
{
    if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
    {
        if (const UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            if (ASC->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().State_Movement_Blocked))
            {
                return 1.f;
            }
        }
    }
    return Super::GetMaxSpeed();
}
