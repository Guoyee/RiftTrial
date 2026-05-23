// Copyright Yerik Guo

#include "AbilitySystem/Abilities/MeleeAttackAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

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
