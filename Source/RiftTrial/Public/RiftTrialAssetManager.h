// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "RiftTrialAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class RIFTTRIAL_API URiftTrialAssetManager : public UAssetManager
{
    GENERATED_BODY()
public:
    
    static URiftTrialAssetManager& Get();
    
protected:
    virtual void StartInitialLoading() override;
    
private:
    
};
