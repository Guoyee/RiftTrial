// Copyright Yerik Guo

#include "AbilitySystem/Abilities/RangedAttackAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "Actor/RangedProjectile.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "RiftTrialGameplayTags.h"
#include "Interaction/CombatInterface.h"

URangedAttackAbility::URangedAttackAbility()
{
}

bool URangedAttackAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
    FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;

    if (const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
    {
        if (ASC->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().Cooldown_Attack)) return false;
    }

    return true;
}

AActor* URangedAttackAbility::GetAttackTarget()
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

void URangedAttackAbility::ApplyAttackCooldown() const
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
                ValidASC->RemoveLooseGameplayTag(FRiftTrialGameplayTags::Get().Cooldown_Attack);
        }), CooldownDuration, false);
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
