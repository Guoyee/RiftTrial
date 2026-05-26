// Copyright Yerik Guo


#include "Character/RiftTrialCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/RiftTrialPlayerController.h"
#include "Player/RiftTrialPlayerState.h"
#include "RiftTrialGameplayTags.h"
#include "UI/HUD/RiftTrialHUD.h"
#include "UI/Widgets/HealthBarComponent.h"

ARiftTrialCharacter::ARiftTrialCharacter()
{
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 800.0f, 0.0f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;

    GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = CameraArmLength;
    CameraBoom->SetRelativeRotation(CameraRotation);
    CameraBoom->bDoCollisionTest = false;
    CameraBoom->bInheritPitch = false;
    CameraBoom->bInheritRoll = false;
    CameraBoom->bInheritYaw = false;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);
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

    // 将 MoveSpeed 属性同步到角色移动速度
    if (URiftTrialAttributeSet* AS = Cast<URiftTrialAttributeSet>(AttributeSet))
    {
        GetCharacterMovement()->MaxWalkSpeed = AS->GetMoveSpeed();

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMoveSpeedAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data)
            {
                GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
            });
    }

    if (HealthBar)
    {
        HealthBar->BindToAbilitySystem(AbilitySystemComponent);
    }

}
