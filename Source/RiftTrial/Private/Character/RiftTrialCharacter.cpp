// Copyright Yerik Guo


#include "Character/RiftTrialCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/RiftTrialPlayerController.h"
#include "Player/RiftTrialPlayerState.h"
#include "UI/HUD/RiftTrialHUD.h"

ARiftTrialCharacter::ARiftTrialCharacter()
{
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 800.0f, 0.0f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;
    
    // 专用服务器上也更新动画姿态，保证技能弹体生成位置正确
    GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

    //角色不跟随摄像机旋转
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;
}

void ARiftTrialCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    
    //Init ability actor info for the Server
    InitAbilityActorInfo();
    AddCharacterAbilities();
}

void ARiftTrialCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    //Init ability actor info for the Client
    InitAbilityActorInfo();
}

int32 ARiftTrialCharacter::GetPlayerLevel()
{
    const ARiftTrialPlayerState* RiftTrialPlayerState = GetPlayerState<ARiftTrialPlayerState>();
    check(RiftTrialPlayerState);
    return RiftTrialPlayerState->GetPlayerLevel();
}

void ARiftTrialCharacter::InitAbilityActorInfo()
{
    ARiftTrialPlayerState* RiftTrialPlayerState = GetPlayerState<ARiftTrialPlayerState>();
    check(RiftTrialPlayerState);
    RiftTrialPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(RiftTrialPlayerState, this);
    Cast<URiftTrialAbilitySystemComponent>(RiftTrialPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
    AbilitySystemComponent = RiftTrialPlayerState->GetAbilitySystemComponent();
    AttributeSet = RiftTrialPlayerState->GetAttributeSet();
    
    if (ARiftTrialPlayerController* RiftTrialPlayerController = Cast<ARiftTrialPlayerController>(GetController()))
    {
        if (ARiftTrialHUD* RiftTrialHUD = Cast<ARiftTrialHUD>(RiftTrialPlayerController->GetHUD()))
        {
            RiftTrialHUD->InitOverlay(RiftTrialPlayerController, RiftTrialPlayerState, AbilitySystemComponent, AttributeSet);
        }
    }
    
    InitializeDefaultAttributes();
    
}
