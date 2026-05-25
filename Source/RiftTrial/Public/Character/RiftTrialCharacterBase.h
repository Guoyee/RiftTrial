// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "RiftTrialCharacterBase.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS(Abstract)
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
    /* end Combat Interface */

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CAT_TEAM)
    int32 TeamID = 0;

    // 死亡动画 Montage
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAT_COMBAT)
    TObjectPtr<UAnimMontage> DeathMontage;

    // 死亡后销毁延迟（秒），0 表示不自动销毁
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAT_COMBAT)
    float DeathDestroyDelay = 5.f;

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayDeathMontage();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = CAT_COMBAT)
    TObjectPtr<USkeletalMeshComponent> Weapon;

    UPROPERTY(EditAnywhere, Category = CAT_COMBAT)
    FName WeaponTipSocketName;

    virtual FVector GetCombatSocketLocation() override;

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UAttributeSet> AttributeSet;

    virtual void InitAbilityActorInfo();

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CAT_ATTRIBUTE)
    TSubclassOf<UGameplayEffect> DefaultAttributes;

    // 身份标签 GE（Type.Minion / Type.Hero），Duration=Infinite
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CAT_ATTRIBUTE)
    TSubclassOf<UGameplayEffect> IdentityEffect;

    void ApplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;

    void InitializeDefaultAttributes() const;

    void AddCharacterAbilities() const;

private:
    UPROPERTY(EditAnywhere, Category = CAT_ABILITY)
    TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};
