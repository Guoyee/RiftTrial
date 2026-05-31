// Copyright Yerik Guo


#include "Character/RiftTrialCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/RiftTrialPlayerController.h"
#include "Player/RiftTrialPlayerState.h"
#include "RiftTrialGameplayTags.h"
#include "UI/HUD/RiftTrialHUD.h"
#include "UI/Widgets/HealthBarComponent.h"

ARiftTrialCharacter::ARiftTrialCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

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

void ARiftTrialCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ARiftTrialCharacter, CurrentAttackTarget);
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
        HealthBar->SetPlayerLevel(GetPlayerLevel());
    }

}

void ARiftTrialCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 指向性技能缓冲（范围外按下 → 走近 → 激活）
    if (BufferedAbilityTarget && IsValid(BufferedAbilityTarget))
    {
        const FVector Dir = (BufferedAbilityTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), Dir.Rotation(), DeltaTime, 15.f));

        const float Dist = FVector::Dist(GetActorLocation(), BufferedAbilityTarget->GetActorLocation());
        if (Dist <= GetAttackRange())
        {
            TryActivateBufferedAbility();
        }
        else
        {
            AddMovementInput(Dir);
        }
        return; // 优先处理缓冲技能，不处理普攻
    }

    if (!CurrentAttackTarget || !IsValid(CurrentAttackTarget))
    {
        return;
    }

    // 目标已死亡 → 清除攻击目标
    if (UAbilitySystemComponent* TargetASC = CurrentAttackTarget->FindComponentByClass<UAbilitySystemComponent>())
    {
        if (TargetASC->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().State_Dead))
        {
            ClearAttackTarget();
            return;
        }
    }

    const float Dist = FVector::Dist(GetActorLocation(), CurrentAttackTarget->GetActorLocation());
    const float Range = GetAttackRange();

    if (Dist > Range)
    {
        // 还在攻击范围外 → 走向目标
        const FVector Dir = (CurrentAttackTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        AddMovementInput(Dir);
    }
    else
    {
        // 在攻击范围内 → 面朝目标，尝试激活普攻
        const FVector Dir = (CurrentAttackTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        const FRotator TargetRotation = Dir.Rotation();
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 15.f));

        TryActivateBasicAttack();
    }
}

void ARiftTrialCharacter::SetAttackTarget(AActor* NewTarget)
{
    // 空目标 / 自己 → 拒绝
    if (!NewTarget || NewTarget == this)
    {
        return;
    }

    // 只有实现 ICombatInterface 的单位可以作为攻击目标
    ICombatInterface* TargetCombat = Cast<ICombatInterface>(NewTarget);
    if (!TargetCombat)
    {
        return;
    }

    // 不能攻击同队目标（中立单位 TeamID=0 可被任意方攻击）
    const int32 MyTeam = GetTeamID();
    const int32 TargetTeam = TargetCombat->GetTeamID();
    if (MyTeam != 0 && TargetTeam == MyTeam)
    {
        return;
    }

    CurrentAttackTarget = NewTarget;
}

void ARiftTrialCharacter::ClearAttackTarget()
{
    CurrentAttackTarget = nullptr;

    // 取消正在运行的普攻 GA（攻击前摇取消）
    if (URiftTrialAbilitySystemComponent* ASC = Cast<URiftTrialAbilitySystemComponent>(AbilitySystemComponent))
    {
        const FGameplayTag InputTag = FRiftTrialGameplayTags::Get().InputTag_RMB;
        for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
        {
            if (Spec.DynamicAbilityTags.HasTagExact(InputTag) && Spec.IsActive())
            {
                ASC->CancelAbilityHandle(Spec.Handle);
            }
        }
    }
}

float ARiftTrialCharacter::GetAttackRange() const
{
    if (const URiftTrialAttributeSet* AS = Cast<URiftTrialAttributeSet>(AttributeSet))
    {
        return AS->GetAttackRange();
    }
    return 150.f; // 默认近战攻击范围
}

void ARiftTrialCharacter::TryActivateBasicAttack()
{
    // 技能激活只在服务端执行
    if (!HasAuthority()) return;

    URiftTrialAbilitySystemComponent* ASC = Cast<URiftTrialAbilitySystemComponent>(AbilitySystemComponent);
    if (!ASC) return;

    const FGameplayTag InputTag = FRiftTrialGameplayTags::Get().InputTag_RMB;

    for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        if (Spec.DynamicAbilityTags.HasTagExact(InputTag) && !Spec.IsActive())
        {
            ASC->TryActivateAbility(Spec.Handle);
            break;
        }
    }
}

int32 ARiftTrialCharacter::IncrementAndGetComboIndex(int32 MontageCount)
{
    if (MontageCount <= 0) return 0;
    const int32 CurrentIndex = AttackComboIndex;
    AttackComboIndex = (CurrentIndex + 1) % MontageCount;
    return CurrentIndex;
}

void ARiftTrialCharacter::OnRep_CurrentAttackTarget()
{
    // 客户端收到同步，无需额外逻辑
}

void ARiftTrialCharacter::SetBufferedAbilityTarget(AActor* Target, FGameplayTag AbilityInputTag)
{
	BufferedAbilityTarget = Target;
	BufferedAbilityInputTag = AbilityInputTag;
}

void ARiftTrialCharacter::ClearBufferedAbilityTarget()
{
	BufferedAbilityTarget = nullptr;
	BufferedAbilityInputTag = FGameplayTag();
}

void ARiftTrialCharacter::TryActivateBufferedAbility()
{
	if (!BufferedAbilityTarget || !IsValid(BufferedAbilityTarget)) return;
	if (!BufferedAbilityInputTag.IsValid()) return;

	URiftTrialAbilitySystemComponent* ASC = Cast<URiftTrialAbilitySystemComponent>(AbilitySystemComponent);
	if (!ASC) return;

	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(BufferedAbilityInputTag) && !Spec.IsActive())
		{
			ASC->TryActivateAbility(Spec.Handle);
			ClearBufferedAbilityTarget();
			return;
		}
	}
}
