// Copyright Yerik Guo


#include "Character/RiftTrialCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/RiftTrialPlayerState.h"
#include "RiftTrial.h"
#include "RiftTrialGameplayTags.h"
#include "TimerManager.h"

// Sets default values
ARiftTrialCharacterBase::ARiftTrialCharacterBase()
{
    PrimaryActorTick.bCanEverTick = false;
    
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetCapsuleComponent()->SetGenerateOverlapEvents(false);
    GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
    GetMesh()->SetGenerateOverlapEvents(true);
    
    Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
    Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
    Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* ARiftTrialCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ARiftTrialCharacterBase::Die()
{
    // 防止重复触发
    if (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().State_Dead))
    {
        return;
    }

    // 添加死亡标签，防止被 AI 继续索敌
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->AddLooseGameplayTag(FRiftTrialGameplayTags::Get().State_Dead);
    }

    // 停止 AI 行为树
    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        AIC->StopMovement();
        if (UBehaviorTreeComponent* BTComp = AIC->FindComponentByClass<UBehaviorTreeComponent>())
        {
            BTComp->StopTree(EBTStopMode::Safe);
        }
    }

    // 停止移动、禁用碰撞
    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 英雄走 PlayerState 复活，小兵直接销毁
    if (ARiftTrialPlayerState* PS = GetPlayerState<ARiftTrialPlayerState>())
    {
        PS->StartRespawnTimer();
    }
    else
    {
        SetLifeSpan(DeathDestroyDelay);
    }

    // 通过 Multicast 在所有客户端播放死亡动画
    MulticastPlayDeathMontage();
}

void ARiftTrialCharacterBase::MulticastPlayDeathMontage_Implementation()
{
    if (DeathMontage && GetMesh() && GetMesh()->GetAnimInstance())
    {
        DeathMontage->bEnableAutoBlendOut = false;
        GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);

        // 死亡动画播完后暂停在最后一帧
        FTimerHandle PauseTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(PauseTimerHandle, [Mesh = GetMesh(), Montage = DeathMontage]()
        {
            if (Mesh && Mesh->GetAnimInstance())
            {
                Mesh->GetAnimInstance()->Montage_Pause(Montage);
            }
        }, DeathMontage->GetPlayLength(), false);
    }
}

void ARiftTrialCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    // 应用身份标签 GE（Type.Minion / Type.Hero），自动复制到客户端
    if (HasAuthority() && IdentityEffect)
    {
        ApplayEffectToSelf(IdentityEffect, 1.f);
    }
}

int32 ARiftTrialCharacterBase::GetTeamID() const
{
    return TeamID;
}

FVector ARiftTrialCharacterBase::GetCombatSocketLocation()
{
    if (Weapon && Weapon->GetSkeletalMeshAsset())
    {
        return Weapon->GetSocketLocation(WeaponTipSocketName);
    }
    // 小兵等没有独立武器模型的单位，从角色 Mesh 的 Socket 获取
    return GetMesh()->GetSocketLocation(WeaponTipSocketName);
}

void ARiftTrialCharacterBase::InitAbilityActorInfo()
{
}

void ARiftTrialCharacterBase::ApplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
    check(IsValid(GetAbilitySystemComponent()));
    check(GameplayEffectClass);
    FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
    ContextHandle.AddSourceObject(this);
    FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ARiftTrialCharacterBase::InitializeDefaultAttributes() const
{
    ApplayEffectToSelf(DefaultAttributes, 1.f);
}

void ARiftTrialCharacterBase::AddCharacterAbilities() const
{
    if (!HasAuthority()) return;
    URiftTrialAbilitySystemComponent* AuraASC = Cast<URiftTrialAbilitySystemComponent>(AbilitySystemComponent);
    AuraASC->AddCharacterAbilities(StartupAbilities);
    
}

