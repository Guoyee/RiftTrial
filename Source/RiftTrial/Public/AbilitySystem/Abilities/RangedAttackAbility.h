// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "RiftTrialDamageGameplayAbility.h"
#include "RangedAttackAbility.generated.h"

class ARangedProjectile;

// 远程攻击 GA：向目标发射追踪弹体
UCLASS(Blueprintable)
class RIFTTRIAL_API URangedAttackAbility : public URiftTrialDamageGameplayAbility
{
    GENERATED_BODY()

public:
    URangedAttackAbility();

    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
        FGameplayTagContainer* OptionalRelevantTags) const override;

protected:
    virtual FVector GetSpawnLocation();

    UFUNCTION(BlueprintCallable)
    void SpawnProjectile(AActor* HomingTarget);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<ARangedProjectile> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float FlightSpeed = 3000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float HitAcceptanceRadius = 50.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bTrackTarget = true;

private:
    void BuildAndSpawnProjectile(AActor* Target);

    FGameplayEffectSpecHandle BuildDamageSpecHandle() const;
};
