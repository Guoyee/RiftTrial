// Copyright Yerik Guo

#include "AI/BTTask_AttackTarget.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "AbilitySystemInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "RiftTrialGameplayTags.h"

UBTTask_AttackTarget::UBTTask_AttackTarget()
{
    NodeName = "Attack Target";
    bNotifyTick = true;

    TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_AttackTarget, TargetKey), AActor::StaticClass());
    TargetKey.SelectedKeyName = "TargetToFollow";
    AttackRangeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_AttackTarget, AttackRangeKey));
    AttackRangeKey.SelectedKeyName = "AttackRange";
}

float GetAttackRange(UBehaviorTreeComponent& OwnerComp, const FBlackboardKeySelector& Key, float DefaultValue)
{
    if (Key.SelectedKeyName.IsNone()) return DefaultValue;

    const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return DefaultValue;

    const float Value = BB->GetValueAsFloat(Key.SelectedKeyName);
    return Value > 0.f ? Value : DefaultValue;
}

EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard) return EBTNodeResult::Failed;

    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
    if (!Target) return EBTNodeResult::Failed;

    return EBTNodeResult::InProgress;
}

void UBTTask_AttackTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
    if (!Target || !IsValid(Target))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 从 GAS 属性读取攻击范围，回退到黑板，最后用配置默认值
    float EffectiveRange = AttackRange;
    if (const IAbilitySystemInterface* ASCI = Cast<IAbilitySystemInterface>(Pawn))
    {
        if (const UAbilitySystemComponent* ASC = ASCI->GetAbilitySystemComponent())
        {
            if (const URiftTrialAttributeSet* AS = Cast<URiftTrialAttributeSet>(ASC->GetAttributeSet(URiftTrialAttributeSet::StaticClass())))
            {
                EffectiveRange = AS->GetAttackRange();
            }
        }
    }
    if (EffectiveRange <= 0.f)
    {
        EffectiveRange = GetAttackRange(OwnerComp, AttackRangeKey, AttackRange);
    }

    const float Dist = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());

    if (Dist > EffectiveRange)
    {
        AIController->MoveToActor(Target, 20.f);
    }
    else
    {
        AIController->StopMovement();

        FVector Dir = Target->GetActorLocation() - Pawn->GetActorLocation();
        Dir.Z = 0.f;
        if (!Dir.IsNearlyZero())
        {
            FRotator TargetRot = Dir.Rotation();
            Pawn->SetActorRotation(FMath::RInterpTo(Pawn->GetActorRotation(), TargetRot, DeltaSeconds, 10.f));
        }

        if (AttackAbilityTag.IsValid())
        {
            IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Pawn);
            if (ASCInterface)
            {
                UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
                if (ASC && !ASC->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().Cooldown_Attack))
                {
                    FGameplayTagContainer TagContainer;
                    TagContainer.AddTag(AttackAbilityTag);
                    ASC->TryActivateAbilitiesByTag(TagContainer);
                }
            }
        }
    }
}

EBTNodeResult::Type UBTTask_AttackTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (AAIController* AIController = OwnerComp.GetAIOwner())
    {
        AIController->StopMovement();
    }
    return EBTNodeResult::Aborted;
}
