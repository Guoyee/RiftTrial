// Copyright Yerik Guo


#include "Game/RiftTrialGameModeBase.h"

void ARiftTrialGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    if (UWorld* World = GetWorld())
    {
        World->GetWorldSettings()->SetTimeDilation(1.0f);
    }
    
    check(RiftTrialCharacterClassInfo);
}
