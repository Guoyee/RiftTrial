// Copyright Yerik Guo

#include "Game/RiftTrialMinionSpawner.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/RiftTrialMinion.h"
#include "Components/SplineComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ARiftTrialMinionSpawner::ARiftTrialMinionSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    LaneSpline = CreateDefaultSubobject<USplineComponent>(TEXT("LaneSpline"));
    SetRootComponent(LaneSpline);
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
}

void ARiftTrialMinionSpawner::SpawnWave()
{
    if (!MinionClass || !LaneSpline) return;

    const FVector SpawnOrigin = LaneSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
    const FVector RightDir = LaneSpline->GetRightVectorAtSplinePoint(0, ESplineCoordinateSpace::World);

    for (int32 i = 0; i < MinionsPerWave; ++i)
    {
        FVector SpawnLoc = SpawnOrigin + RightDir * (i * 150.f);
        FRotator SpawnRot = LaneSpline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::World);

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        ARiftTrialMinion* Minion = GetWorld()->SpawnActorDeferred<ARiftTrialMinion>(
            MinionClass,
            FTransform(SpawnRot, SpawnLoc),
            this,
            nullptr,
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

        if (!Minion) continue;

        Minion->TeamID = TeamID;
        Minion->FinishSpawning(FTransform(SpawnRot, SpawnLoc));

        // 下一帧写黑板（等自动 Possess + RunBehaviorTree 完成）
        FTimerHandle InitBTTimer;
        GetWorldTimerManager().SetTimer(InitBTTimer, [Minion, this]()
        {
            AAIController* AIC = Cast<AAIController>(Minion->GetController());
            if (AIC && AIC->GetBlackboardComponent())
            {
                AIC->GetBlackboardComponent()->SetValueAsObject(SplineActorBBKey, this);
                AIC->GetBlackboardComponent()->SetValueAsFloat(SplineDistanceBBKey, 0.f);
            }
        }, 0.1f, false);
    }
}
