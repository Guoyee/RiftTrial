// Copyright Yerik Guo

#include "AI/BTService_FindNearestEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/RiftTrialCharacterBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"

UBTService_FindNearestEnemy::UBTService_FindNearestEnemy()
{
    NodeName = "Find Nearest Enemy";

    TargetToFollowSelector.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FindNearestEnemy, TargetToFollowSelector), AActor::StaticClass());
    DistanceToTargetSelector.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FindNearestEnemy, DistanceToTargetSelector));

    TargetToFollowSelector.SelectedKeyName = "TargetToFollow";
    DistanceToTargetSelector.SelectedKeyName = "DistanceToTarget";
}

void UBTService_FindNearestEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard) return;

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    APawn* MyPawn = AIController->GetPawn();
    if (!MyPawn) return;

    const ICombatInterface* MyCombat = Cast<ICombatInterface>(MyPawn);
    if (!MyCombat) return;

    const int32 MyTeamID = MyCombat->GetTeamID();
    if (MyTeamID == 0)
    {
        Blackboard->ClearValue(TargetToFollowSelector.SelectedKeyName);
        Blackboard->SetValueAsFloat(DistanceToTargetSelector.SelectedKeyName, 0.f);
        return;
    }

    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARiftTrialCharacterBase::StaticClass(), AllActors);

    float NearestDist = AggroRange > 0.f ? AggroRange : FLT_MAX;
    AActor* NearestEnemy = nullptr;

    for (AActor* Actor : AllActors)
    {
        if (Actor == MyPawn || !IsValid(Actor)) continue;

        const ICombatInterface* OtherCombat = Cast<ICombatInterface>(Actor);
        if (!OtherCombat) continue;

        if (OtherCombat->GetTeamID() == MyTeamID) continue;

        const float Dist = FVector::Dist(MyPawn->GetActorLocation(), Actor->GetActorLocation());
        if (Dist < NearestDist)
        {
            NearestDist = Dist;
            NearestEnemy = Actor;
        }
    }

    if (NearestEnemy)
    {
        Blackboard->SetValueAsObject(TargetToFollowSelector.SelectedKeyName, NearestEnemy);
        Blackboard->SetValueAsFloat(DistanceToTargetSelector.SelectedKeyName, NearestDist);
    }
    else
    {
        Blackboard->ClearValue(TargetToFollowSelector.SelectedKeyName);
        Blackboard->SetValueAsFloat(DistanceToTargetSelector.SelectedKeyName, 0.f);
    }
}
