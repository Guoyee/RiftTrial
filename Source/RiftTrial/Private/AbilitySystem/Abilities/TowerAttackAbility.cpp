// Copyright Yerik Guo

#include "AbilitySystem/Abilities/TowerAttackAbility.h"
#include "Actor/RiftTrialTower.h"

AActor* UTowerAttackAbility::GetAttackTarget()
{
    if (ARiftTrialTower* Tower = Cast<ARiftTrialTower>(GetAvatarActorFromActorInfo()))
    {
        return Tower->SelectTarget();
    }
    return Super::GetAttackTarget();
}
