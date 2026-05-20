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

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USplineComponent> LaneSpline;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<ARiftTrialMinion> MinionClass;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    int32 TeamID = 1;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    float SpawnInterval = 30.f;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    int32 MinionsPerWave = 6;

    UPROPERTY(EditAnywhere, Category = "AI")
    FName SplineActorBBKey = "SplinePathActor";

    UPROPERTY(EditAnywhere, Category = "AI")
    FName SplineDistanceBBKey = "SplineDistance";

private:
    void SpawnWave();

    FTimerHandle SpawnTimerHandle;
};
