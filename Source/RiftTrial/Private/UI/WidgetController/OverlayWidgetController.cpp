// Copyright Yerik Guo


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/RiftTrialAbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "Player/RiftTrialPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
    const URiftTrialAttributeSet* RiftTrialAttributeSet = Cast<URiftTrialAttributeSet>(AttributeSet);
    
    OnHealthChanged.Broadcast(RiftTrialAttributeSet->GetHealth());
    OnMaxHealthChanged.Broadcast(RiftTrialAttributeSet->GetMaxHealth());
    
    OnManaChanged.Broadcast(RiftTrialAttributeSet->GetMana());
    OnMaxManaChanged.Broadcast(RiftTrialAttributeSet->GetMaxMana());

    const ARiftTrialPlayerState* PS = Cast<ARiftTrialPlayerState>(PlayerState);
    OnPlayerLevelChanged.Broadcast(PS ? PS->GetPlayerLevel() : 1);
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
    const URiftTrialAttributeSet* RiftTrialAttributeSet = Cast<URiftTrialAttributeSet>(AttributeSet);
    
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RiftTrialAttributeSet->GetHealthAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data)
            {
                OnHealthChanged.Broadcast(Data.NewValue);
            }
        );
    
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RiftTrialAttributeSet->GetMaxHealthAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data)
            {
                OnMaxHealthChanged.Broadcast(Data.NewValue);
            }
        );

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RiftTrialAttributeSet->GetManaAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data)
            {
                OnManaChanged.Broadcast(Data.NewValue);
            }
        );
    
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RiftTrialAttributeSet->GetMaxManaAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data)
            {
                OnMaxManaChanged.Broadcast(Data.NewValue);
            }
        );
    
    Cast<URiftTrialAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
        [this](const FGameplayTagContainer& AssetTags)
        {
            for (const FGameplayTag& Tag : AssetTags)
            {
                //"A.1".MatchesTag("A") will return True, "A".MatchesTag("A.1") will return False
                FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
                if (Tag.MatchesTag(MessageTag))
                {
                const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
                MessageWidgetRowDelegate.Broadcast(*Row);
                }
                
            }
        }
    );
    
}

