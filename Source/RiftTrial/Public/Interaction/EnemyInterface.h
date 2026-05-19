// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * 
 */
class RIFTTRIAL_API IEnemyInterface
{
    GENERATED_BODY()

public:
    virtual void HighlightActor(int32 StencilValue) = 0;
    virtual void UnHighlightActor() = 0;
};
