// Copyright Yerik Guo


#include "Character/RiftTrialCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "RiftTrial.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ARiftTrialCharacterBase::ARiftTrialCharacterBase()
{
    PrimaryActorTick.bCanEverTick = false;
    
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetCapsuleComponent()->SetGenerateOverlapEvents(false);
    GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
    GetMesh()->SetGenerateOverlapEvents(true);
    
    Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
    Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
    Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* ARiftTrialCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

UAnimMontage* ARiftTrialCharacterBase::GetHitReactMontage_Implementation()
{
    return HitReactMontage;
}

void ARiftTrialCharacterBase::Die()
{
    Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
    MulticastHandleDeath();
}

//该函数会在客户端和服务端调用
void ARiftTrialCharacterBase::MulticastHandleDeath_Implementation()
{
    Weapon->SetSimulatePhysics(true);
    Weapon->SetEnableGravity(true);
    Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetEnableGravity(true);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    OnDeath();
    Dissolve();
}

void ARiftTrialCharacterBase::BeginPlay()
{
    Super::BeginPlay();
    
}

FVector ARiftTrialCharacterBase::GetCombatSocketLocation()
{
    check(Weapon);
    return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void ARiftTrialCharacterBase::InitAbilityActorInfo()
{
}

void ARiftTrialCharacterBase::ApplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
    check(IsValid(GetAbilitySystemComponent()));
    check(GameplayEffectClass);
    FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
    ContextHandle.AddSourceObject(this);
    FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ARiftTrialCharacterBase::InitializeDefaultAttributes() const
{
    ApplayEffectToSelf(DefaultPrimaryAttributes, 1.f);
    ApplayEffectToSelf(DefaultSecondaryAttributes, 1.f);
    ApplayEffectToSelf(DefaultVitalAttributes, 1.f);
}

void ARiftTrialCharacterBase::AddCharacterAbilities() const
{
    if (!HasAuthority()) return;
    URiftTrialAbilitySystemComponent* AuraASC = Cast<URiftTrialAbilitySystemComponent>(AbilitySystemComponent);
    AuraASC->AddCharacterAbilities(StartupAbilities);
    
}

void ARiftTrialCharacterBase::Dissolve()
{
    if (IsValid(DissolveMaterialInstance))
    {
        UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
        GetMesh()->SetMaterial(0, DynamicMatInst);
        StartDissolveTimeline(DynamicMatInst);
    }
    
    if (IsValid(WeaponDissolveMaterialInstance))
    {
        UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
        Weapon->SetMaterial(0, DynamicMatInst);
        StartWeaponDissolveTimeline(DynamicMatInst);
    }
}




