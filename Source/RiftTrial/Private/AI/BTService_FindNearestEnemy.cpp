// Copyright Yerik Guo

#include "AI/BTService_FindNearestEnemy.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Actor/RiftTrialTower.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/RiftTrialCharacterBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "RiftTrialGameplayTags.h"

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

    // 锁定目标：如果当前目标仍有效且未脱离索敌范围，保持锁定不切换
    AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetToFollowSelector.SelectedKeyName));
    if (IsValid(CurrentTarget))
    {
        // 跳过已死亡的目标
        const IAbilitySystemInterface* CurrentASC = Cast<IAbilitySystemInterface>(CurrentTarget);
        if (CurrentASC && CurrentASC->GetAbilitySystemComponent())
        {
            if (!CurrentASC->GetAbilitySystemComponent()->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().State_Dead))
            {
                const ICombatInterface* CurrentCombat = Cast<ICombatInterface>(CurrentTarget);
                if (CurrentCombat && CurrentCombat->GetTeamID() != MyTeamID)
                {
                    const float CurrentDist = FVector::Dist(MyPawn->GetActorLocation(), CurrentTarget->GetActorLocation());

                    if (AggroRange <= 0.f || CurrentDist <= AggroRange)
                    {
                        // 目标仍有效，只更新距离，不换目标
                        Blackboard->SetValueAsFloat(DistanceToTargetSelector.SelectedKeyName, CurrentDist);
                        return;
                    }
                }
            }
        }
    }

    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARiftTrialCharacterBase::StaticClass(), AllActors);

    // 同时搜索防御塔
    TArray<AActor*> TowerActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARiftTrialTower::StaticClass(), TowerActors);
    AllActors.Append(TowerActors);

    float NearestDist = AggroRange > 0.f ? AggroRange : FLT_MAX;
    AActor* NearestEnemy = nullptr;

    for (AActor* Actor : AllActors)
    {
        if (Actor == MyPawn || !IsValid(Actor)) continue;

        // 跳过已死亡的目标
        const IAbilitySystemInterface* OtherASC = Cast<IAbilitySystemInterface>(Actor);
        if (OtherASC && OtherASC->GetAbilitySystemComponent())
        {
            if (OtherASC->GetAbilitySystemComponent()->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().State_Dead)) continue;
        }

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
