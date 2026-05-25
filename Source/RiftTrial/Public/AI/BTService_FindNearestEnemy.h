// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "RiftTrial.h"
#include "BTService_FindNearestEnemy.generated.h"

UCLASS()
class RIFTTRIAL_API UBTService_FindNearestEnemy : public UBTService
{
    GENERATED_BODY()

public:
    UBTService_FindNearestEnemy();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|AI")
    FBlackboardKeySelector TargetToFollowSelector;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|AI")
    FBlackboardKeySelector DistanceToTargetSelector;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|AI")
    float AggroRange = 0.f;
};
