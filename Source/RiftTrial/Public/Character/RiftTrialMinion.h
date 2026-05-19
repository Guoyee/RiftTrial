// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Character/RiftTrialCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "RiftTrialMinion.generated.h"

UCLASS()
class RIFTTRIAL_API ARiftTrialMinion : public ARiftTrialCharacterBase, public IEnemyInterface
{
    GENERATED_BODY()

public:
    ARiftTrialMinion();

    // EnemyInterface
    virtual void HighlightActor(int32 StencilValue) override;
    virtual void UnHighlightActor() override;
    // end EnemyInterface

protected:
    virtual void BeginPlay() override;
    virtual void InitAbilityActorInfo() override;
};
