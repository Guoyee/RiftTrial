// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RangedAttackAbility.h"
#include "TowerAttackAbility.generated.h"

// 防御塔攻击 GA：GetAttackTarget 委托给 Tower::SelectTarget
UCLASS(Blueprintable)
class RIFTTRIAL_API UTowerAttackAbility : public URangedAttackAbility
{
    GENERATED_BODY()

public:
    virtual AActor* GetAttackTarget() override;
};
