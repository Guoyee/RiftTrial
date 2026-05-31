// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Character/RiftTrialCharacterBase.h"
#include "GameplayTagContainer.h"
#include "RiftTrial.h"
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
    virtual void Tick(float DeltaTime) override;

    /* Combat Interface*/
    virtual int32 GetPlayerLevel() override;
    /* end Combat Interface*/

    FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    // 设置 / 清除当前攻击目标（由 PlayerController 在 RMB 点击时调用）
    UFUNCTION(BlueprintCallable, Category = "RiftTrial|Combat")
    void SetAttackTarget(AActor* NewTarget);
    UFUNCTION(BlueprintCallable, Category = "RiftTrial|Combat")
    void ClearAttackTarget();
    FORCEINLINE AActor* GetCurrentAttackTarget() const { return CurrentAttackTarget; }

    // 从 GAS Attribute 读取攻击范围，默认 150.f
    float GetAttackRange() const;

    // 攻击连击索引（用于蒙太奇轮播），非 AI 黑板的 fallback
    int32 AttackComboIndex = 0;
    int32 IncrementAndGetComboIndex(int32 MontageCount);

    // 指向性技能缓冲目标（范围外按下技能时存储，走近后激活）
    void SetBufferedAbilityTarget(AActor* Target, FGameplayTag AbilityInputTag);
    void ClearBufferedAbilityTarget();
    FORCEINLINE AActor* GetBufferedAbilityTarget() const { return BufferedAbilityTarget; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RiftTrial|Camera")
    float CameraArmLength = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RiftTrial|Camera")
    FRotator CameraRotation = FRotator(-60.f, -90.f, 0.f);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UCameraComponent> FollowCamera;

    // 当前攻击目标（由 PlayerController 设置，Tick 中用于持续攻击）
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentAttackTarget, Category = "RiftTrial|Combat")
    TObjectPtr<AActor> CurrentAttackTarget;

    UFUNCTION()
    void OnRep_CurrentAttackTarget();

    // 尝试激活普攻 GA（检查冷却 + 目标是否在范围内）
    void TryActivateBasicAttack();

    AActor* BufferedAbilityTarget = nullptr;
    FGameplayTag BufferedAbilityInputTag;

    void TryActivateBufferedAbility();
};
