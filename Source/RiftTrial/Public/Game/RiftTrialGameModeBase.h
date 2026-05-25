// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RiftTrial.h"
#include "RiftTrialGameModeBase.generated.h"

class URiftTrialCharacterClassInfo;
/**
 * 
 */
UCLASS()
class RIFTTRIAL_API ARiftTrialGameModeBase : public AGameModeBase
{
    GENERATED_BODY()
protected:
    virtual void BeginPlay() override;
    
public:
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Attribute")
    TObjectPtr<URiftTrialCharacterClassInfo> RiftTrialCharacterClassInfo;
};
