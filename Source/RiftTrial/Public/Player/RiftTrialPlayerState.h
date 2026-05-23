// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "RiftTrialPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
/**
 * 
 */
UCLASS()
class RIFTTRIAL_API ARiftTrialPlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()
public:
    ARiftTrialPlayerState();
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UAttributeSet* GetAttributeSet() const {return AttributeSet;}
    
    FORCEINLINE int32 GetPlayerLevel() const { return Level; }
    FORCEINLINE int32 GetAvailableSkillPoints() const { return AvailableSkillPoints; }
    void AddSkillPoint() { ++AvailableSkillPoints; }
    void SpendSkillPoint() { if (AvailableSkillPoints > 0) --AvailableSkillPoints; }

    // 启动复活倒计时
    void StartRespawnTimer();

    UPROPERTY(EditDefaultsOnly, Category = "Respawn")
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
