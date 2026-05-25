// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FollowSplinePath.generated.h"

UCLASS()
class RIFTTRIAL_API UBTTask_FollowSplinePath : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_FollowSplinePath();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual FString GetStaticDescription() const override;

    UPROPERTY(EditAnywhere, Category = CAT_AI)
    FBlackboardKeySelector SplineActorKey;

    UPROPERTY(EditAnywhere, Category = CAT_AI)
    FBlackboardKeySelector SplineDistanceKey;

    UPROPERTY(EditAnywhere, Category = CAT_AI)
    float DefaultSpeed = 200.f;
};
