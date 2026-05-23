// Copyright Yerik Guo

#include "AI/BTTask_FollowSplinePath.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "AbilitySystemInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Pawn.h"

UBTTask_FollowSplinePath::UBTTask_FollowSplinePath()
{
    NodeName = "Follow Spline Path";
    bNotifyTick = true;

    SplineActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FollowSplinePath, SplineActorKey), AActor::StaticClass());
    SplineDistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FollowSplinePath, SplineDistanceKey));

    SplineActorKey.SelectedKeyName = "SplinePathActor";
    SplineDistanceKey.SelectedKeyName = "SplineDistance";
}

FString UBTTask_FollowSplinePath::GetStaticDescription() const
{
    return FString::Printf(TEXT("Move along spline from '%s'"),
        *SplineActorKey.SelectedKeyName.ToString());
}

EBTNodeResult::Type UBTTask_FollowSplinePath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    return EBTNodeResult::InProgress;
}

void UBTTask_FollowSplinePath::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return;

    // 从 GAS 属性读取移速，未配则用默认值
    float MoveSpeed = DefaultSpeed;
    if (const IAbilitySystemInterface* ASCI = Cast<IAbilitySystemInterface>(Pawn))
    {
        if (const UAbilitySystemComponent* ASC = ASCI->GetAbilitySystemComponent())
        {
            if (const URiftTrialAttributeSet* AS = Cast<URiftTrialAttributeSet>(ASC->GetAttributeSet(URiftTrialAttributeSet::StaticClass())))
            {
                MoveSpeed = AS->GetMoveSpeed();
            }
        }
    }
    if (MoveSpeed <= 0.f) MoveSpeed = DefaultSpeed;

    AActor* SplineActor = Cast<AActor>(Blackboard->GetValueAsObject(SplineActorKey.SelectedKeyName));
    if (!SplineActor)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    USplineComponent* Spline = SplineActor->FindComponentByClass<USplineComponent>();
    if (!Spline || Spline->GetNumberOfSplinePoints() == 0)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    const float CurrentDist = Blackboard->GetValueAsFloat(SplineDistanceKey.SelectedKeyName);
    const float NewDist = CurrentDist + MoveSpeed * DeltaSeconds;
    const float SplineLength = Spline->GetSplineLength();

    if (NewDist >= SplineLength)
    {
        Blackboard->SetValueAsFloat(SplineDistanceKey.SelectedKeyName, SplineLength);
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    const FVector Location = Spline->GetLocationAtDistanceAlongSpline(NewDist, ESplineCoordinateSpace::World);
    const FRotator Rotation = Spline->GetRotationAtDistanceAlongSpline(NewDist, ESplineCoordinateSpace::World);

    Pawn->SetActorLocation(Location);
    Pawn->SetActorRotation(Rotation);

    Blackboard->SetValueAsFloat(SplineDistanceKey.SelectedKeyName, NewDist);
}
