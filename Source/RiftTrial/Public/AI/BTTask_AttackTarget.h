// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BTTask_AttackTarget.generated.h"

UCLASS()
class RIFTTRIAL_API UBTTask_AttackTarget : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_AttackTarget();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    UPROPERTY(EditAnywhere, Category = CAT_AI)
    FBlackboardKeySelector TargetKey;

    UPROPERTY(EditAnywhere, Category = CAT_AI)
    FBlackboardKeySelector AttackRangeKey;

    UPROPERTY(EditAnywhere, Category = CAT_AI)
    float AttackRange = 400.f;

    UPROPERTY(EditAnywhere, Category = CAT_ABILITY)
    FGameplayTag AttackAbilityTag;
};
