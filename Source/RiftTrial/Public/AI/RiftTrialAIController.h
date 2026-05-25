// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RiftTrialAIController.generated.h"

class UBehaviorTree;
class UBehaviorTreeComponent;

UCLASS()
class RIFTTRIAL_API ARiftTrialAIController : public AAIController
{
    GENERATED_BODY()

public:
    ARiftTrialAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(EditAnywhere, Category = CAT_AI)
    TObjectPtr<UBehaviorTree> BehaviorTree;

    TObjectPtr<UBlackboardComponent> BlackboardComponent;

    TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
