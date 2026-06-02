// Copyright Yerik Guo


#include "Player/RiftTrialPlayerState.h"

#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "Character/RiftTrialCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "Net/UnrealNetwork.h"
#include "RiftTrialGameplayTags.h"
#include "TimerManager.h"

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
    DOREPLIFETIME(ARiftTrialPlayerState, AvailableSkillPoints);
}

UAbilitySystemComponent* ARiftTrialPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ARiftTrialPlayerState::OnRep_Level(int32 OldLevel)
{

}

void ARiftTrialPlayerState::OnRep_AvailableSkillPoints(int32 OldValue)
{

}

void ARiftTrialPlayerState::StartRespawnTimer()
{
    GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &ARiftTrialPlayerState::RespawnPawn, RespawnDelay, false);
}

void ARiftTrialPlayerState::RespawnPawn()
{
    ACharacter* MyChar = Cast<ACharacter>(GetPawn());
    if (!MyChar) return;

    // 移除死亡 GE（同时移除 State.Dead 标签，自动同步）
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        FGameplayTagContainer TagsToRemove;
        TagsToRemove.AddTag(FRiftTrialGameplayTags::Get().State_Dead);
        ASC->RemoveActiveEffectsWithGrantedTags(TagsToRemove);
    }

    // 恢复移动和碰撞
    MyChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    MyChar->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MyChar->SetLifeSpan(0.f);

    // 恢复动画：停止死亡蒙太奇、取消暂停
    if (UAnimInstance* AnimInstance = MyChar->GetMesh()->GetAnimInstance())
    {
        if (ARiftTrialCharacterBase* CharBase = Cast<ARiftTrialCharacterBase>(MyChar))
        {
            if (CharBase->DeathMontage)
            {
                AnimInstance->Montage_Stop(0.f, CharBase->DeathMontage);
            }
        }
    }
    MyChar->GetMesh()->bPauseAnims = false;

    // 回满血量
    if (URiftTrialAttributeSet* AS = Cast<URiftTrialAttributeSet>(GetAttributeSet()))
    {
        AS->SetHealth(AS->GetMaxHealth());
        AS->SetMana(AS->GetMaxMana());
    }

    // 传送到复活点
    if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
    {
        if (AActor* Start = GM->FindPlayerStart(Cast<AController>(GetOwner())))
        {
            MyChar->SetActorLocation(Start->GetActorLocation());
            MyChar->SetActorRotation(Start->GetActorRotation());
        }
    }
}
