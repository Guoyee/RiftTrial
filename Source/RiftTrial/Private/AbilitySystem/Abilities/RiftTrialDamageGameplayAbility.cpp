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
