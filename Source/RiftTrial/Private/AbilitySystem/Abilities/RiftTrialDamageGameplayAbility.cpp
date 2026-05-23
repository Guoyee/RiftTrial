// Copyright Yerik Guo

#include "AbilitySystem/Abilities/RiftTrialDamageGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "RiftTrialGameplayTags.h"
#include "TimerManager.h"

AActor* URiftTrialDamageGameplayAbility::GetAttackTarget()
{
    if (AActor* Avatar = GetAvatarActorFromActorInfo())
    {
        if (APawn* Pawn = Cast<APawn>(Avatar))
        {
            if (AAIController* AIC = Cast<AAIController>(Pawn->GetController()))
            {
                if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
                {
                    AActor* Target = Cast<AActor>(BB->GetValueAsObject("TargetToFollow"));
                    return IsValid(Target) ? Target : nullptr;
                }
            }
        }
    }

    return nullptr;
}

void URiftTrialDamageGameplayAbility::ApplyAttackCooldown()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC) return;

    float AttackSpeed = 1.f;
    if (const URiftTrialAttributeSet* AS = Cast<URiftTrialAttributeSet>(ASC->GetAttributeSet(URiftTrialAttributeSet::StaticClass())))
    {
        AttackSpeed = AS->GetAttackSpeed();
        AttackSpeed = FMath::Max(AttackSpeed, 0.01f);
    }

    const float CooldownDuration = 1.0f / AttackSpeed;

    ASC->AddLooseGameplayTag(FRiftTrialGameplayTags::Get().Cooldown_Attack);

    UWorld* World = GetWorld();
    if (!World)
    {
        ASC->RemoveLooseGameplayTag(FRiftTrialGameplayTags::Get().Cooldown_Attack);
        return;
    }

    FTimerHandle TimerHandle;
    World->GetTimerManager().SetTimer(TimerHandle,
        FTimerDelegate::CreateLambda([WeakASC = TWeakObjectPtr<UAbilitySystemComponent>(ASC)]
        {
            if (UAbilitySystemComponent* ValidASC = WeakASC.Get())
            {
                ValidASC->RemoveLooseGameplayTag(FRiftTrialGameplayTags::Get().Cooldown_Attack);
            }
        }), CooldownDuration, false);
}

UAnimMontage* URiftTrialDamageGameplayAbility::GetNextAttackMontage()
{
    if (AttackMontages.IsEmpty()) return nullptr;

    // 从黑板读取当前连击索引
    int32 Index = 0;
    if (AActor* Avatar = GetAvatarActorFromActorInfo())
    {
        if (APawn* Pawn = Cast<APawn>(Avatar))
        {
            if (AAIController* AIC = Cast<AAIController>(Pawn->GetController()))
            {
                if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
                {
                    Index = BB->GetValueAsInt("ComboIndex");
                }
            }
        }
    }

    UAnimMontage* Montage = AttackMontages[Index].Get();

    // 写入下一个索引
    const int32 NextIndex = (Index + 1) % AttackMontages.Num();
    if (AActor* Avatar = GetAvatarActorFromActorInfo())
    {
        if (APawn* Pawn = Cast<APawn>(Avatar))
        {
            if (AAIController* AIC = Cast<AAIController>(Pawn->GetController()))
            {
                if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
                {
                    BB->SetValueAsInt("ComboIndex", NextIndex);
                }
            }
        }
    }

    return Montage;
}
