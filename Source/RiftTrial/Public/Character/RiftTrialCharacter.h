// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Character/RiftTrialCharacterBase.h"
#include "RiftTrialCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RIFTTRIAL_API ARiftTrialCharacter : public ARiftTrialCharacterBase
{
    GENERATED_BODY()
    
public:
    ARiftTrialCharacter();
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;
    virtual void InitAbilityActorInfo() override;
    
    /* Combat Interface*/
    virtual int32 GetPlayerLevel() override;
    /* end Combat Interface*/
    
private:
};
