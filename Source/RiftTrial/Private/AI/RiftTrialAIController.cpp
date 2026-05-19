// Copyright Yerik Guo

#include "AI/RiftTrialAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

ARiftTrialAIController::ARiftTrialAIController()
{
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
    check(BlackboardComponent);
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
    check(BehaviorTreeComponent);
}

void ARiftTrialAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if (BehaviorTree)
    {
        RunBehaviorTree(BehaviorTree);
    }
}
