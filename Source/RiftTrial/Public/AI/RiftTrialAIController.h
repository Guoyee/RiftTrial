// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RiftTrialAIController.generated.h"

class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class RIFTTRIAL_API ARiftTrialAIController : public AAIController
{
	GENERATED_BODY()
	
public:
    
    ARiftTrialAIController();
    
protected:
    
    TObjectPtr<UBlackboardComponent> BlackboardComponent;
    
    TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
