// Copyright Yerik Guo


#include "Player/RiftTrialPlayerState.h"

#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "Net/UnrealNetwork.h"

ARiftTrialPlayerState::ARiftTrialPlayerState()
{
    AbilitySystemComponent = CreateDefaultSubobject<URiftTrialAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
    
    AttributeSet = CreateDefaultSubobject<URiftTrialAttributeSet>("AttributeSet");
    NetUpdateFrequency = 100.f;
}

void ARiftTrialPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ARiftTrialPlayerState, Level);
}

UAbilitySystemComponent* ARiftTrialPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ARiftTrialPlayerState::OnRep_Level(int32 OldLevel)
{
    
}
