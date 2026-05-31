// Copyright Yerik Guo

#include "AbilitySystem/Abilities/RiftTrialDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/RiftTrialCharacter.h"
#include "GameFramework/Pawn.h"
#include "RiftTrialGameplayTags.h"
#include "TimerManager.h"

AActor* URiftTrialDamageGameplayAbility::GetAttackTarget()
{
    if (AActor* Avatar = GetAvatarActorFromActorInfo())
    {
        if (APawn* Pawn = Cast<APawn>(Avatar))
        {
            // 优先从 AI 黑板读取（小兵 / 塔）
            if (AAIController* AIC = Cast<AAIController>(Pawn->GetController()))
            {
                if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
                {
                    AActor* Target = Cast<AActor>(BB->GetValueAsObject("TargetToFollow"));
                    if (IsValid(Target))
                    {
                        return Target;
                    }
                }
            }

            // 玩家英雄：从角色属性读取当前攻击目标
            if (const ARiftTrialCharacter* Hero = Cast<ARiftTrialCharacter>(Pawn))
            {
                return Hero->GetCurrentAttackTarget();
            }
        }
    }

    return nullptr;
}

UAnimMontage* URiftTrialDamageGameplayAbility::GetNextAttackMontage()
{
    if (AttackMontages.IsEmpty()) return nullptr;

    int32 Index = 0;
    bool bIndexFound = false;

    if (AActor* Avatar = GetAvatarActorFromActorInfo())
    {
        if (APawn* Pawn = Cast<APawn>(Avatar))
        {
            // 优先从 AI 黑板读取（小兵 / 塔）
            if (AAIController* AIC = Cast<AAIController>(Pawn->GetController()))
            {
                if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
                {
                    Index = BB->GetValueAsInt("ComboIndex");
                    bIndexFound = true;

                    // 写入下一个索引到黑板
                    const int32 NextIndex = (Index + 1) % AttackMontages.Num();
                    BB->SetValueAsInt("ComboIndex", NextIndex);
                }
            }

            // 玩家英雄：从角色属性读取并递增连击索引
            if (!bIndexFound)
            {
                if (ARiftTrialCharacter* Hero = Cast<ARiftTrialCharacter>(Pawn))
                {
                    Index = Hero->IncrementAndGetComboIndex(AttackMontages.Num());
                    bIndexFound = true;
                }
            }
        }
    }

    UAnimMontage* Montage = AttackMontages[Index].Get();
    return Montage;
}

float URiftTrialDamageGameplayAbility::GetAttackSpeed() const
{
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        if (const URiftTrialAttributeSet* AS = Cast<URiftTrialAttributeSet>(ASC->GetAttributeSet(URiftTrialAttributeSet::StaticClass())))
        {
            return FMath::Max(AS->GetAttackSpeed(), 0.1f); // 最低 0.1 防止反向播放
        }
    }
    return 1.0f;
}

void URiftTrialDamageGameplayAbility::ApplyDamageToTarget(AActor* Target)
{
    // 伤害计算只能在服务端执行
    if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
    if (!IsValid(Target) || !DamageEffectClass) return;

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!SourceASC) return;

    UAbilitySystemComponent* TargetASC = Target->FindComponentByClass<UAbilitySystemComponent>();
    if (!TargetASC) return;

    FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
    EffectContext.SetAbility(this);
    EffectContext.AddSourceObject(GetAvatarActorFromActorInfo());

    FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContext);
    if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("ApplyDamageToTarget: Failed to create GE spec"));
        return;
    }

    for (const auto& Pair : DamageTypes)
    {
        const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
    }

    SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}
