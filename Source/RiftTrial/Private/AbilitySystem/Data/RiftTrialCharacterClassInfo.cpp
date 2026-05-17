// Copyright Yerik Guo


#include "AbilitySystem/Data/RiftTrialCharacterClassInfo.h"

FCharacterClassDefaultInfo URiftTrialCharacterClassInfo::GetClassDefaultInfo(ECharacterClass characterClass)
{
    return CharacterClassInformation.FindChecked(characterClass);
}
