// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RiftTrialMinionSpawner.generated.h"

class ARiftTrialMinion;
class USplineComponent;

UCLASS()
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

    UPROPERTY(EditAnywhere, Category = CAT_SPAWNING)
    TSubclassOf<ARiftTrialMinion> MeleeClass;

    UPROPERTY(EditAnywhere, Category = CAT_SPAWNING)
    TSubclassOf<ARiftTrialMinion> RangedClass;

    UPROPERTY(EditAnywhere, Category = CAT_SPAWNING)
    int32 TeamID = 1;

    UPROPERTY(EditAnywhere, Category = CAT_SPAWNING)
    float SpawnInterval = 30.f;

    UPROPERTY(EditAnywhere, Category = CAT_SPAWNING)
    int32 MeleeCount = 3;

    UPROPERTY(EditAnywhere, Category = CAT_SPAWNING)
    int32 RangedCount = 3;

    UPROPERTY(EditAnywhere, Category = CAT_SPAWNING)
    float SpawnDelayBetweenMinions = 0.5f;

    UPROPERTY(EditAnywhere, Category = CAT_AI)
    FName SplineActorBBKey = "SplinePathActor";

    UPROPERTY(EditAnywhere, Category = CAT_AI)
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
