// Copyright Yerik Guo

#include "Game/RiftTrialMinionSpawner.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/RiftTrialMinion.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ARiftTrialMinionSpawner::ARiftTrialMinionSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    SpawnerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SpawnerMesh"));
    SetRootComponent(SpawnerMesh);

    LaneSpline = CreateDefaultSubobject<USplineComponent>(TEXT("LaneSpline"));
    LaneSpline->SetupAttachment(SpawnerMesh);
}

void ARiftTrialMinionSpawner::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ARiftTrialMinionSpawner::SpawnWave, SpawnInterval, true, 0.f);
    }
}

void ARiftTrialMinionSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
    GetWorldTimerManager().ClearTimer(SpawnStepTimer);
}

void ARiftTrialMinionSpawner::SpawnWave()
{
    if (!LaneSpline) return;

    SpawnOrigin = LaneSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
    SpawnRot = LaneSpline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::World);
    SpawnIndex = 0;
    SpawnMeleeRemaining = MeleeCount;
    SpawnRangedRemaining = RangedCount;

    SpawnNextMinion();
}

void ARiftTrialMinionSpawner::SpawnNextMinion()
{
    TSubclassOf<ARiftTrialMinion> Class = nullptr;

    if (SpawnMeleeRemaining > 0)
    {
        Class = MeleeClass;
        --SpawnMeleeRemaining;
    }
    else if (SpawnRangedRemaining > 0)
    {
        Class = RangedClass;
        --SpawnRangedRemaining;
    }

    if (!Class)
    {
        return; // 本轮波次全部生成完毕
    }

    ARiftTrialMinion* Minion = GetWorld()->SpawnActorDeferred<ARiftTrialMinion>(
        Class, FTransform(SpawnRot, SpawnOrigin), this, nullptr,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

    if (Minion)
    {
        Minion->TeamID = TeamID;
        Minion->FinishSpawning(FTransform(SpawnRot, SpawnOrigin));

        AAIController* AIC = Cast<AAIController>(Minion->GetController());
        if (AIC && AIC->GetBlackboardComponent())
        {
            AIC->GetBlackboardComponent()->SetValueAsObject(SplineActorBBKey, this);
            AIC->GetBlackboardComponent()->SetValueAsFloat(SplineDistanceBBKey, 0.f);
        }
    }

    // 下一个小兵延迟生成
    ++SpawnIndex;
    GetWorldTimerManager().SetTimer(SpawnStepTimer, this,
        &ARiftTrialMinionSpawner::SpawnNextMinion, SpawnDelayBetweenMinions, false);
}
