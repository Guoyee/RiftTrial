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
    UAttributeSet* GetAttributeSet() const {return AttributeSet;}
    
    /* Combat Interface */
    virtual void Die() override;
    virtual int32 GetTeamID() const override;
    /* end Combat Interface */

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
    int32 TeamID = 0;

    // 死亡动画 Montage
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UAnimMontage> DeathMontage;

    // 死亡后销毁延迟（秒），0 表示不自动销毁
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float DeathDestroyDelay = 5.f;

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayDeathMontage();
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Combat")
    TObjectPtr<USkeletalMeshComponent> Weapon;

    UPROPERTY(EditAnywhere, Category = "Combat")
    FName WeaponTipSocketName;

    virtual FVector GetCombatSocketLocation() override;

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UAttributeSet> AttributeSet;

    virtual void InitAbilityActorInfo();

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attribute")
    TSubclassOf<UGameplayEffect> DefaultAttributes;

    void ApplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;

    void InitializeDefaultAttributes() const;

    void AddCharacterAbilities() const;

private:
    UPROPERTY(EditAnywhere, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};
