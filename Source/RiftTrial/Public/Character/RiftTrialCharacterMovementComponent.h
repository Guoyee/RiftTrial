// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RiftTrialCharacterMovementComponent.generated.h"

UCLASS()
class RIFTTRIAL_API URiftTrialCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    virtual float GetMaxSpeed() const override;
};
