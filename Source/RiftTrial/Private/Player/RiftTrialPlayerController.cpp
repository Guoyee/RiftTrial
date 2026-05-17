// Copyright Yerik Guo


#include "Player/RiftTrialPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "RiftTrialGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "MaterialHLSLTree.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Input/RiftTrialInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "UI/Widgets/DamageTextComponent.h"

ARiftTrialPlayerController::ARiftTrialPlayerController()
{
    bReplicates = true;
    
    Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void ARiftTrialPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    
    CursorTrace();
    AutoRun();
}

void ARiftTrialPlayerController::ShowDamageNumber_Implementation(ACharacter* TargetCharacter, float DamageAmount, bool bBlockedHit, bool bCriticalHit)
{
    if (IsValid(TargetCharacter) && DamageTextComponentClass)
    {
        UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
        DamageText->RegisterComponent();
        DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        // [Claude] 随机偏移避免多次伤害跳字重叠
        DamageText->SetRelativeLocation(FVector(FMath::RandRange(-30.f, 30.f), 0.f, FMath::RandRange(0.f, 40.f)));

        DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
    }
}

void ARiftTrialPlayerController::AutoRun()
{
    if (!bAutoRunning) return;
    if (APawn* ControlledPawn = GetPawn())
    {
        const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
        const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
        ControlledPawn->AddMovementInput(Direction);
        
        const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
        if (DistanceToDestination < AutoRunAcceptanceRadius)
        {
            bAutoRunning = false;
        }
    }
}

void ARiftTrialPlayerController::CursorTrace()
{
    GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
    if (!CursorHit.bBlockingHit) return;
    
    LastActor = ThisActor;
    ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
    
    if (LastActor!=ThisActor)
    {
        if (LastActor) LastActor->UnHighlightActor();
        if (ThisActor) ThisActor->HighlightActor();
    }
}

void ARiftTrialPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    if (InputTag.MatchesTagExact(FRiftTrialGameplayTags::Get().InputTag_RMB))
    {
        bTargeting = ThisActor ? true : false;
        bAutoRunning = false;
    }
    if (InputTag.MatchesTagExact(FRiftTrialGameplayTags::Get().InputTag_LMB)) bAutoRunning = false;
}

void ARiftTrialPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
    if (!InputTag.MatchesTagExact(FRiftTrialGameplayTags::Get().InputTag_RMB))
    {
        if (GetASC())
        {
            GetASC()->AbilityInputTagReleased(InputTag);
        }
        return;
    }
    
    if (bTargeting)
    {
        if (GetASC())
        {
            GetASC()->AbilityInputTagReleased(InputTag);
        }
        bTargeting = false;
    }else
    {
        if (FollowTime <= ShortPressThreshold)
        {
            if (APawn* ControlledPawn = GetPawn())
            {
                UNavigationPath* NaviPath = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), ControlledPawn->GetActorLocation(), CachedDestination);
                if (NaviPath != nullptr)
                {
                    Spline->ClearSplinePoints();
                    for (const FVector& PointLoc : NaviPath->PathPoints)
                    {
                        Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
                    }
                    bAutoRunning = true;
                    
                    if (NaviPath ->PathPoints.Num() > 0)
                    {
                        CachedDestination = NaviPath->PathPoints.Last();
                    }
                }

            }
        }
        FollowTime = 0.f;
    }
    
}

void ARiftTrialPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    if (!InputTag.MatchesTagExact(FRiftTrialGameplayTags::Get().InputTag_RMB))
    {
        if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);

        return;
    }
    
    if (bTargeting)
    {
        if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
    }else
    {
        FollowTime += GetWorld()->GetDeltaSeconds();
        
        if (CursorHit.bBlockingHit)
        {
            CachedDestination = CursorHit.ImpactPoint;
        }
        
        if (APawn* ControlledPawn = GetPawn())
        {
            const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
            ControlledPawn->AddMovementInput(WorldDirection);
        }
    }
    
}

URiftTrialAbilitySystemComponent* ARiftTrialPlayerController::GetASC()
{
    if (RiftTrialAbilitySystemComponent == nullptr)
    {
        RiftTrialAbilitySystemComponent = Cast<URiftTrialAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
    }
    return RiftTrialAbilitySystemComponent;
}


void ARiftTrialPlayerController::BeginPlay()
{
    Super::BeginPlay();
    check(AuraContext);
    
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    if (Subsystem)
    {
    Subsystem->AddMappingContext(AuraContext, 0);
    }
    
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    
    FInputModeGameAndUI InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputModeData.SetHideCursorDuringCapture(false);
    SetInputMode(InputModeData);
}

void ARiftTrialPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    URiftTrialInputComponent* RiftTrialInputComponent = CastChecked<URiftTrialInputComponent>(InputComponent);
    
    RiftTrialInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARiftTrialPlayerController::Move);
    
    //调用RiftTrialInputComponent中定义的模板函数来完成BindAction
    RiftTrialInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void ARiftTrialPlayerController::Move(const FInputActionValue& InputActionValue)
{
    const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
    
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    
    if (APawn* ControlledPawn = GetPawn<APawn>())
    {
        ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
        ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
    }
    
}


