// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "RiftTrialPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class RIFTTRIAL_API ARiftTrialPlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ARiftTrialPlayerState();
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UAttributeSet* GetAttributeSet() const { return AttributeSet; }

    FORCEINLINE int32 GetPlayerLevel() const { return Level; }
    FORCEINLINE int32 GetAvailableSkillPoints() const { return AvailableSkillPoints; }
    void AddSkillPoint() { ++AvailableSkillPoints; }
    void SpendSkillPoint() { if (AvailableSkillPoints > 0) --AvailableSkillPoints; }

    void StartRespawnTimer();

    UPROPERTY(EditDefaultsOnly, Category = CAT_COMBAT)
    float RespawnDelay = 10.f;

protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UAttributeSet> AttributeSet;

private:
    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
    int32 Level = 1;

    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AvailableSkillPoints)
    int32 AvailableSkillPoints = 0;

    UFUNCTION()
    void OnRep_Level(int32 OldLevel);

    UFUNCTION()
    void OnRep_AvailableSkillPoints(int32 OldValue);

    void RespawnPawn();

    FTimerHandle RespawnTimer;
};
