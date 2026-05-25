// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RiftTrial.h"
#include "RiftTrialPlayerController.generated.h"


class UDamageTextComponent;
class USplineComponent;
class URiftTrialAbilitySystemComponent;
struct FGameplayTag;
class URiftTrialInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;


/**
 * 
 */
UCLASS(PrioritizeCategories = "RiftTrial")
class RIFTTRIAL_API ARiftTrialPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    ARiftTrialPlayerController();
    virtual void PlayerTick(float DeltaTime) override;
    
    //播放伤害数字只在本地（Controller对应的客户端）进行，由Sever调用RPC，或者本地直接运行
    //需要传入收到伤害的Character的指针，damage widget附着在该对象上
    UFUNCTION(Client, Reliable)
    void ShowDamageNumber(ACharacter* TargetCharacter, float DamageAmount, bool bBlockedHit, bool bCriticalHit);
    
protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    
private:
    UPROPERTY(EditAnywhere, Category = "RiftTrial|Input")
    TObjectPtr<UInputMappingContext> RTContext;
    
    UPROPERTY(EditAnywhere, Category = "RiftTrial|Input")
    TObjectPtr<UInputAction> MoveAction;
    
    void Move(const FInputActionValue& InputActionValue);
    
    void CursorTrace();
    IEnemyInterface* LastActor;
    IEnemyInterface* ThisActor;
    FHitResult CursorHit;
    
    void AbilityInputTagPressed(FGameplayTag InputTag);
    void AbilityInputTagReleased(FGameplayTag InputTag);
    void AbilityInputTagHeld(FGameplayTag InputTag);
    
    UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Input")
    TObjectPtr<URiftTrialInputConfig> InputConfig;
    
    TObjectPtr<URiftTrialAbilitySystemComponent> RiftTrialAbilitySystemComponent;
    
    URiftTrialAbilitySystemComponent* GetASC();
    
    FVector CachedDestination = FVector::ZeroVector;
    float FollowTime = 0.f;
    float ShortPressThreshold = 0.5f;
    bool bAutoRunning = false;
    bool bTargeting = false;
    
    UPROPERTY(EditDefaultsOnly)
    float AutoRunAcceptanceRadius = 20.f;
    
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USplineComponent> Spline;
    
    void AutoRun();
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};
