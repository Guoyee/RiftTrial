// Copyright Yerik Guo

#include "Character/RiftTrialMinion.h"
#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "RiftTrial.h"

ARiftTrialMinion::ARiftTrialMinion()
{
    AbilitySystemComponent = CreateDefaultSubobject<URiftTrialAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    AttributeSet = CreateDefaultSubobject<URiftTrialAttributeSet>("AttributeSet");
}

void ARiftTrialMinion::BeginPlay()
{
    Super::BeginPlay();
    InitAbilityActorInfo();
    InitializeDefaultAttributes();
    AddCharacterAbilities();
}

void ARiftTrialMinion::InitAbilityActorInfo()
{
    AbilitySystemComponent->InitAbilityActorInfo(this, this);
    Cast<URiftTrialAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
}

void ARiftTrialMinion::HighlightActor()
{
    GetMesh()->SetRenderCustomDepth(true);
    GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void ARiftTrialMinion::UnHighlightActor()
{
    GetMesh()->SetRenderCustomDepth(false);
}
