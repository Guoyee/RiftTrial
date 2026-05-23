// Copyright Yerik Guo


#include "RiftTrialAssetManager.h"
#include "RiftTrialGameplayTags.h"

URiftTrialAssetManager& URiftTrialAssetManager::Get()
{
    check(GEngine)
    URiftTrialAssetManager* RiftTrialAssetManager = Cast<URiftTrialAssetManager>(GEngine->AssetManager);
    return *RiftTrialAssetManager;
}

void URiftTrialAssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();
    FRiftTrialGameplayTags::InitializeNativeGameplayTags();
}
