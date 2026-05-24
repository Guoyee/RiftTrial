// Copyright Yerik Guo

#include "AbilitySystem/Abilities/RangedAttackAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "Actor/RangedProjectile.h"
#include "RiftTrialGameplayTags.h"
#include "Interaction/CombatInterface.h"

URangedAttackAbility::URangedAttackAbility()
{
}

bool URangedAttackAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
    FGameplayTagContainer* OptionalRelevantTags) const
{
    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
    // 冷却由 Cooldown GE 自动处理，无需手动检查
}

FVector URangedAttackAbility::GetSpawnLocation()
{
    if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
    {
        return CombatInterface->GetCombatSocketLocation();
    }
    return GetAvatarActorFromActorInfo()->GetActorLocation();
}

void URangedAttackAbility::SpawnProjectile(AActor* HomingTarget)
{
    BuildAndSpawnProjectile(HomingTarget);
}

FGameplayEffectSpecHandle URangedAttackAbility::BuildDamageSpecHandle() const
{
    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!SourceASC) return FGameplayEffectSpecHandle();

    FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
    EffectContextHandle.SetAbility(this);
    EffectContextHandle.AddSourceObject(this);

    FHitResult HitResult;
    HitResult.Location = const_cast<URangedAttackAbility*>(this)->GetSpawnLocation();
    EffectContextHandle.AddHitResult(HitResult);

    const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

    for (const auto& Pair : DamageTypes)
    {
        const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
    }

    return SpecHandle;
}

void URangedAttackAbility::BuildAndSpawnProjectile(AActor* Target)
{
    if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

    const FVector SpawnLocation = GetSpawnLocation();
    const FRotator SpawnRotation = (Target->GetActorLocation() - SpawnLocation).Rotation();

    FTransform SpawnTransform;
    SpawnTransform.SetLocation(SpawnLocation);
    SpawnTransform.SetRotation(SpawnRotation.Quaternion());

    ARangedProjectile* Projectile = GetWorld()->SpawnActorDeferred<ARangedProjectile>(
        ProjectileClass,
        SpawnTransform,
        GetAvatarActorFromActorInfo(),
        Cast<APawn>(GetAvatarActorFromActorInfo()),
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    Projectile->DamageEffectSpecHandle = BuildDamageSpecHandle();
    Projectile->FlightSpeed = FlightSpeed;
    Projectile->HitAcceptanceRadius = HitAcceptanceRadius;
    Projectile->bTrackTarget = bTrackTarget;
    Projectile->SetTarget(Target);
    Projectile->FinishSpawning(SpawnTransform);
}
