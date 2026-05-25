// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RiftTrial.h"
#include "RiftTrialMinionSpawner.generated.h"

class ARiftTrialMinion;
class USplineComponent;

UCLASS(PrioritizeCategories = "RiftTrial")
class RIFTTRIAL_API ARiftTrialMinionSpawner : public AActor
{
    GENERATED_BODY()

public:
    ARiftTrialMinionSpawner();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USkeletalMeshComponent> SpawnerMesh;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USplineComponent> LaneSpline;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|Spawning")
    TSubclassOf<ARiftTrialMinion> MeleeClass;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|Spawning")
    TSubclassOf<ARiftTrialMinion> RangedClass;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|Spawning")
    int32 TeamID = 1;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|Spawning")
    float SpawnInterval = 30.f;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|Spawning")
    int32 MeleeCount = 3;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|Spawning")
    int32 RangedCount = 3;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|Spawning")
    float SpawnDelayBetweenMinions = 0.5f;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|AI")
    FName SplineActorBBKey = "SplinePathActor";

    UPROPERTY(EditAnywhere, Category = "RiftTrial|AI")
    FName SplineDistanceBBKey = "SplineDistance";

private:
    void SpawnWave();
    void SpawnNextMinion();

    FTimerHandle SpawnTimerHandle;
    FTimerHandle SpawnStepTimer;

    FVector SpawnOrigin;
    FRotator SpawnRot;
    int32 SpawnIndex = 0;
    int32 SpawnMeleeRemaining = 0;
    int32 SpawnRangedRemaining = 0;
};
