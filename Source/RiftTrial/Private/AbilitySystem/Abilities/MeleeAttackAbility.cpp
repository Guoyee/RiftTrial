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
                    AActor* Target = Cast<AActor>(BB->GetValueAsObject("TargetToFollow"));
                    return IsValid(Target) ? Target : nullptr;
                }
            }
        }
    }

    return nullptr;
}

UAnimMontage* UMeleeAttackAbility::GetNextAttackMontage()
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

void UMeleeAttackAbility::ApplyDamageToTarget(AActor* Target)
{
    // 伤害计算只能在服务端执行，客户端仅播放动画
    if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
    if (!IsValid(Target) || !DamageEffectClass) return;

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

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("ApplyAttackCooldown: GetWorld() is null, cannot set timer"));
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
