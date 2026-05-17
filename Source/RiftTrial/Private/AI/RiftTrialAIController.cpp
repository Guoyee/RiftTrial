// Copyright Yerik Guo


#include "AI/RiftTrialAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

ARiftTrialAIController::ARiftTrialAIController()
{
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
    check(BlackboardComponent);
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
    check(BehaviorTreeComponent);
}
