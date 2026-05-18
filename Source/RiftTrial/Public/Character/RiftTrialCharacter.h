// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Character/RiftTrialCharacterBase.h"
#include "RiftTrialCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class RIFTTRIAL_API ARiftTrialCharacter : public ARiftTrialCharacterBase
{
    GENERATED_BODY()

public:
    ARiftTrialCharacter();
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;
    virtual void InitAbilityActorInfo() override;

    /* Combat Interface*/
    virtual int32 GetPlayerLevel() override;
    /* end Combat Interface*/

    FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraArmLength = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FRotator CameraRotation = FRotator(-60.f, -90.f, 0.f);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UCameraComponent> FollowCamera;
};
