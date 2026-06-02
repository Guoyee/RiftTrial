// Copyright Yerik Guo

#include "Actor/RiftTrialTower.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "AbilitySystemInterface.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "RiftTrialGameplayTags.h"
#include "UI/Widgets/HealthBarComponent.h"

ARiftTrialTower::ARiftTrialTower()
{
    PrimaryActorTick.bCanEverTick = false;

    TowerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TowerMesh"));
    SetRootComponent(TowerMesh);

    CursorCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CursorCapsule"));
    CursorCapsule->SetupAttachment(TowerMesh);
    CursorCapsule->SetCapsuleHalfHeight(200.f);
    CursorCapsule->SetCapsuleRadius(100.f);
    CursorCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CursorCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
    CursorCapsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
    DetectionSphere->SetupAttachment(TowerMesh);
    DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    DetectionSphere->SetSphereRadius(800.f);

    AbilitySystemComponent = CreateDefaultSubobject<URiftTrialAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    AttributeSet = CreateDefaultSubobject<URiftTrialAttributeSet>(TEXT("AttributeSet"));

    HealthBar = CreateDefaultSubobject<UHealthBarComponent>("HealthBar");
    HealthBar->SetupAttachment(TowerMesh);
    HealthBar->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
}

void ARiftTrialTower::BeginPlay()
{
    Super::BeginPlay();

    DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ARiftTrialTower::OnBeginOverlap);
    DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ARiftTrialTower::OnEndOverlap);

    AbilitySystemComponent->InitAbilityActorInfo(this, this);
    Cast<URiftTrialAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
    InitializeAttributes();

    if (HealthBar)
    {
        HealthBar->BindToAbilitySystem(AbilitySystemComponent);
    }

    // 应用身份标签 GE（Type.Tower）
    if (HasAuthority() && IdentityEffect)
    {
        FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
        FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(IdentityEffect, 1.f, ContextHandle);
        if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
        {
            AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }

    AddAbilities();
}

UAbilitySystemComponent* ARiftTrialTower::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ARiftTrialTower::InitializeAttributes() const
{
    if (!DefaultAttributes) return;

    FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
    ContextHandle.AddSourceObject(const_cast<ARiftTrialTower*>(this));

    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1.f, ContextHandle);
    if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
    {
        AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}

int32 ARiftTrialTower::GetTeamID() const
{
    return TeamID;
}

FVector ARiftTrialTower::GetCombatSocketLocation()
{
    if (ProjectileSocketName.IsNone())
    {
        return GetActorLocation();
    }
    return TowerMesh->GetSocketLocation(ProjectileSocketName);
}

void ARiftTrialTower::Die()
{
    if (bIsDead) return;
    bIsDead = true;

    if (AbilitySystemComponent)
    {
        if (DeathEffect)
        {
            FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
            ContextHandle.AddSourceObject(this);
            FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DeathEffect, 1.f, ContextHandle);
            if (SpecHandle.IsValid())
            {
                AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            }
        }
        AbilitySystemComponent->CancelAllAbilities();
    }

    DetectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(5.f);
}

void ARiftTrialTower::HighlightActor(int32 StencilValue)
{
    TowerMesh->SetRenderCustomDepth(true);
    TowerMesh->SetCustomDepthStencilValue(StencilValue);
}

void ARiftTrialTower::UnHighlightActor()
{
    TowerMesh->SetRenderCustomDepth(false);
}

AActor* ARiftTrialTower::SelectTarget()
{
    AActor* AggroedTarget = nullptr;
    AActor* MinionTarget = nullptr;
    AActor* HeroTarget = nullptr;

    for (AActor* Actor : OverlappingActors)
    {
        if (!IsValid(Actor) || Actor == this) continue;

        if (const IAbilitySystemInterface* ASCI = Cast<IAbilitySystemInterface>(Actor))
        {
            if (const UAbilitySystemComponent* ASC = ASCI->GetAbilitySystemComponent())
            {
                if (ASC->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().State_Dead)) continue;
            }
        }

        // 跳过己方
        if (const ICombatInterface* Combat = Cast<ICombatInterface>(Actor))
        {
            if (Combat->GetTeamID() == TeamID) continue;
        }

        // 塔不打塔
        if (const IAbilitySystemInterface* ASCI = Cast<IAbilitySystemInterface>(Actor))
        {
            if (const UAbilitySystemComponent* ASC = ASCI->GetAbilitySystemComponent())
            {
                if (ASC->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().Type_Tower)) continue;
            }
        }

        // 优先 Aggroed
        if (const IAbilitySystemInterface* ASCI = Cast<IAbilitySystemInterface>(Actor))
        {
            if (const UAbilitySystemComponent* ASC = ASCI->GetAbilitySystemComponent())
            {
                if (ASC->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().State_Aggroed) && !AggroedTarget)
                {
                    AggroedTarget = Actor;
                    continue;
                }
            }
        }

        // 次选 Minion
        if (const IAbilitySystemInterface* ASCI = Cast<IAbilitySystemInterface>(Actor))
        {
            if (const UAbilitySystemComponent* ASC = ASCI->GetAbilitySystemComponent())
            {
                if (ASC->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().Type_Minion) && !MinionTarget)
                {
                    MinionTarget = Actor;
                    continue;
                }
            }
        }

        // 最后 Hero
        if (const IAbilitySystemInterface* ASCI = Cast<IAbilitySystemInterface>(Actor))
        {
            if (const UAbilitySystemComponent* ASC = ASCI->GetAbilitySystemComponent())
            {
                if (ASC->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().Type_Hero) && !HeroTarget)
                {
                    HeroTarget = Actor;
                }
            }
        }
    }

    if (AggroedTarget) return AggroedTarget;
    if (MinionTarget) return MinionTarget;
    return HeroTarget;
}

void ARiftTrialTower::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    // 只追踪有 ASC 的目标
    if (!Cast<IAbilitySystemInterface>(OtherActor)) return;

    OverlappingActors.AddUnique(OtherActor);
}

void ARiftTrialTower::AddAbilities() const
{
    if (!HasAuthority()) return;
    Cast<URiftTrialAbilitySystemComponent>(AbilitySystemComponent)->AddCharacterAbilities(StartupAbilities);
}

void ARiftTrialTower::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    OverlappingActors.Remove(OtherActor);
}
