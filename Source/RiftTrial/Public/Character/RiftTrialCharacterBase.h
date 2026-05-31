// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "RiftTrial.h"
#include "RiftTrialCharacterBase.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;
class UHealthBarComponent;

UCLASS(Abstract, PrioritizeCategories = "RiftTrial")
class RIFTTRIAL_API ARiftTrialCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
    GENERATED_BODY()

public:
    ARiftTrialCharacterBase();
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UAttributeSet* GetAttributeSet() const { return AttributeSet; }

    /* Combat Interface */
    virtual void Die() override;
    virtual int32 GetTeamID() const override;
    virtual void HighlightActor(int32 StencilValue) override;
    virtual void UnHighlightActor() override;
    /* end Combat Interface */

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RiftTrial|Team")
    int32 TeamID = 0;

    // 死亡动画 Montage
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RiftTrial|Combat")
    TObjectPtr<UAnimMontage> DeathMontage;

    // 死亡后销毁延迟（秒），0 表示不自动销毁
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RiftTrial|Combat")
    float DeathDestroyDelay = 5.f;

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayDeathMontage();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RiftTrial|UI")
    TObjectPtr<UHealthBarComponent> HealthBar;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|Combat")
    TObjectPtr<USkeletalMeshComponent> Weapon;

    UPROPERTY(EditAnywhere, Category = "RiftTrial|Combat")
    FName WeaponTipSocketName;

    virtual FVector GetCombatSocketLocation() override;

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UAttributeSet> AttributeSet;

    virtual void InitAbilityActorInfo();

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "RiftTrial|Attribute")
    TSubclassOf<UGameplayEffect> DefaultAttributes;

    // 身份标签 GE（Type.Minion / Type.Hero），Duration=Infinite
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "RiftTrial|Attribute")
    TSubclassOf<UGameplayEffect> IdentityEffect;

    void ApplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;

    void InitializeDefaultAttributes() const;

    void AddCharacterAbilities() const;

private:
    UPROPERTY(EditAnywhere, Category = "RiftTrial|Ability")
    TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};
