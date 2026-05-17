// Copyright Yerik Guo


#include "AbilitySystem/RiftTrialAbilitySystemLibrary.h"

#include "RiftTrialAbilityTypes.h"
#include "AbilitySystem/Data/RiftTrialCharacterClassInfo.h"
#include "Game/RiftTrialGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RiftTrialPlayerState.h"
#include "UI/HUD/RiftTrialHUD.h"
#include "UI/WidgetController/RiftTrialWidgetController.h"

UOverlayWidgetController* URiftTrialAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        if (ARiftTrialHUD* RiftTrialHUD = Cast<ARiftTrialHUD>(PC->GetHUD()))
        {
            ARiftTrialPlayerState* PS = PC->GetPlayerState<ARiftTrialPlayerState>();
            UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
            UAttributeSet* AS = PS->GetAttributeSet();
            const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
            return RiftTrialHUD->GetOverlayWidgetController(WidgetControllerParams);
        }
    }
    
    return nullptr;
}

UAttributeMenuWidgetController* URiftTrialAbilitySystemLibrary::GetAttributeMenuWidgetController(
    const UObject* WorldContextObject)
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        if (ARiftTrialHUD* RiftTrialHUD = Cast<ARiftTrialHUD>(PC->GetHUD()))
        {
            ARiftTrialPlayerState* PS = PC->GetPlayerState<ARiftTrialPlayerState>();
            UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
            UAttributeSet* AS = PS->GetAttributeSet();
            const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
            return RiftTrialHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
        }
    }
    
    return nullptr;
}

URiftTrialCharacterClassInfo* URiftTrialAbilitySystemLibrary::GetRiftTrialCharacterClassInfo(const UObject* WorldContextObject)
{
    ARiftTrialGameModeBase* AuraGameMode = Cast<ARiftTrialGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
    if (AuraGameMode == nullptr) return nullptr;
    
    return AuraGameMode->RiftTrialCharacterClassInfo;
}

bool URiftTrialAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
    if (const FRiftTrialGameplayEffectContext* AuraEffectContext = static_cast<const FRiftTrialGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return AuraEffectContext->IsBlockedHit();
    }
    return false;
}

bool URiftTrialAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
    if (const FRiftTrialGameplayEffectContext* AuraEffectContext = static_cast<const FRiftTrialGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return AuraEffectContext->IsCriticalHit();
    }
    return false;
}

void URiftTrialAbilitySystemLibrary::SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlocked)
{
    if (FRiftTrialGameplayEffectContext* AuraEffectContext = static_cast<FRiftTrialGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        AuraEffectContext->SetIsBlockedHit(bInIsBlocked);
    }
}

void URiftTrialAbilitySystemLibrary::SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
    if (FRiftTrialGameplayEffectContext* AuraEffectContext = static_cast<FRiftTrialGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit);
    }
}
