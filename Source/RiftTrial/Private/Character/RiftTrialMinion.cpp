// Copyright Yerik Guo

#include "Character/RiftTrialMinion.h"
#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RiftTrial.h"
#include "RiftTrialGameplayTags.h"
#include "UI/Widgets/HealthBarComponent.h"

ARiftTrialMinion::ARiftTrialMinion()
{
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);

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

    // 将 MoveSpeed 属性同步到角色移动速度
    if (URiftTrialAttributeSet* AS = Cast<URiftTrialAttributeSet>(AttributeSet))
    {
        GetCharacterMovement()->MaxWalkSpeed = AS->GetMoveSpeed();

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMoveSpeedAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data)
            {
                GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
            });
    }

    if (HealthBar)
    {
        HealthBar->BindToAbilitySystem(AbilitySystemComponent);
    }
}

