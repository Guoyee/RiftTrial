// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindNearestEnemy.generated.h"

UCLASS()
class RIFTTRIAL_API UBTService_FindNearestEnemy : public UBTService
{
    GENERATED_BODY()

public:
    UBTService_FindNearestEnemy();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetToFollowSelector;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector DistanceToTargetSelector;

    UPROPERTY(EditAnywhere, Category = "Config")
    float AggroRange = 0.f;
};
