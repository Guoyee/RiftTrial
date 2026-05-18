// Copyright Yerik Guo


#include "Game/RiftTrialGameModeBase.h"

void ARiftTrialGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    if (UWorld* World = GetWorld())
    {
        World->GetWorldSettings()->SetTimeDilation(1.0f);
    }
    // TODO: 等需要按职业查表时再配 DA_CharacterClassInfo
    // check(RiftTrialCharacterClassInfo);
}
