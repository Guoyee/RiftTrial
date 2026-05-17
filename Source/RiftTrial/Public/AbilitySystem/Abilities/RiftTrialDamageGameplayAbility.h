// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RiftTrialGameplayAbility.h"
#include "RiftTrialDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class RIFTTRIAL_API URiftTrialDamageGameplayAbility : public URiftTrialGameplayAbility
{
	GENERATED_BODY()
	
    
protected:
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UGameplayEffect> DamageEffectClass;
    
    
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
