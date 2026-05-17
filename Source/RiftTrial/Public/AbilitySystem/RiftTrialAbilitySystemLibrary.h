// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RiftTrialAbilitySystemLibrary.generated.h"

struct FGameplayEffectContextHandle;
class URiftTrialCharacterClassInfo;
enum class ECharacterClass : uint8;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class RIFTTRIAL_API URiftTrialAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    
    UFUNCTION(BlueprintPure, Category="RiftTrialAbilitySystemLibrary|WidgetController")
    static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
    
    UFUNCTION(BlueprintPure, Category="RiftTrialAbilitySystemLibrary|WidgetController")
    static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
    
    UFUNCTION(BlueprintCallable, Category = "RiftTrialAbilitySystemLibrary|CharacterClassDefauts")
    static URiftTrialCharacterClassInfo* GetRiftTrialCharacterClassInfo(const UObject* WorldContextObject);
    
    UFUNCTION(BlueprintPure, Category = "RiftTrialAbilitySystemLibrary|GameplayEffects")
    static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);
    
    UFUNCTION(BlueprintPure, Category = "RiftTrialAbilitySystemLibrary|GameplayEffects")
    static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);
    
    //这两个Set函数在此处做封装的意义存疑，因为目前只会在ExecCalc_Damage中Set这些值
    UFUNCTION(BlueprintCallable, Category = "RiftTrialAbilitySystemLibrary|GameplayEffects")
    static void SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);
    
    UFUNCTION(BlueprintCallable, Category = "RiftTrialAbilitySystemLibrary|GameplayEffects")
    static void SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);
};
