// Copyright Yerik Guo

#include "Character/RiftTrialMinion.h"
#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "RiftTrial.h"

ARiftTrialMinion::ARiftTrialMinion()
{
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

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

void ARiftTrialMinion::HighlightActor(int32 StencilValue)
{
    GetMesh()->SetRenderCustomDepth(true);
    GetMesh()->SetCustomDepthStencilValue(StencilValue);
}

void ARiftTrialMinion::UnHighlightActor()
{
    GetMesh()->SetRenderCustomDepth(false);
}
