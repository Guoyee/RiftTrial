// Copyright Yerik Guo

#include "AbilitySystem/Abilities/MeleeAttackAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "RiftTrialGameplayTags.h"
#include "TimerManager.h"

AActor* UMeleeAttackAbility::GetAttackTarget()
{
    if (AActor* Avatar = GetAvatarActorFromActorInfo())
    {
        if (APawn* Pawn = Cast<APawn>(Avatar))
        {
            if (AAIController* AIC = Cast<AAIController>(Pawn->GetController()))
            {
                if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
                {
                    return Cast<AActor>(BB->GetValueAsObject("TargetToFollow"));
                }
            }
        }
    }

    return nullptr;
}

void UMeleeAttackAbility::ApplyDamageToTarget(AActor* Target)
{
    if (!Target || !DamageEffectClass) return;

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!SourceASC) return;

    UAbilitySystemComponent* TargetASC = nullptr;
    if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Target))
    {
        TargetASC = ASCInterface->GetAbilitySystemComponent();
    }
    if (!TargetASC) return;

    FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
    EffectContext.SetAbility(this);
    EffectContext.AddSourceObject(GetAvatarActorFromActorInfo());

    FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContext);

    for (const auto& Pair : DamageTypes)
    {
        const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
    }

    SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

void UMeleeAttackAbility::ApplyAttackCooldown()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC) return;

    // 读取攻速属性，计算冷却时长：CD = 基础间隔 / 攻速
    float AttackSpeed = 1.f;
    if (const URiftTrialAttributeSet* AS = Cast<URiftTrialAttributeSet>(ASC->GetAttributeSet(URiftTrialAttributeSet::StaticClass())))
    {
        AttackSpeed = AS->GetAttackSpeed();
        AttackSpeed = FMath::Max(AttackSpeed, 0.01f);
    }

    const float CooldownDuration = 1.0f / AttackSpeed;

    // 添加冷却 Tag，阻止再次激活 GA
    ASC->AddLooseGameplayTag(FRiftTrialGameplayTags::Get().Cooldown_Attack);

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle,
        FTimerDelegate::CreateLambda([WeakASC = TWeakObjectPtr<UAbilitySystemComponent>(ASC)]
        {
            if (UAbilitySystemComponent* ValidASC = WeakASC.Get())
                ValidASC->RemoveLooseGameplayTag(FRiftTrialGameplayTags::Get().Cooldown_Attack);
        }), CooldownDuration, false);
}
