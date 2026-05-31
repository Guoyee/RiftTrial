// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Character/RiftTrialCharacterBase.h"
#include "RiftTrialMinion.generated.h"

UCLASS()
class RIFTTRIAL_API ARiftTrialMinion : public ARiftTrialCharacterBase
{
    GENERATED_BODY()

public:
    ARiftTrialMinion();

protected:
    virtual void BeginPlay() override;
    virtual void InitAbilityActorInfo() override;
};
