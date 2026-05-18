// Copyright Yerik Guo


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "RiftTrialGameplayTags.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "AbilitySystem/Data/UAttributeInfo.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{    
    URiftTrialAttributeSet* AS = CastChecked<URiftTrialAttributeSet>(AttributeSet);
    for (auto& Pair : AS->TagsToAttributeMap)
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
            [this, Pair](const FOnAttributeChangeData & Data)
            {
                FRiftTrialAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(Pair.Key);
                Info.AttributeValue = Pair.Value().GetNumericValue(AttributeSet);
                AttributeInfoDelegate.Broadcast(Info);
            }
            );
    }
}


void UAttributeMenuWidgetController::BroadcastInitialValues()
{
    URiftTrialAttributeSet* AS = CastChecked<URiftTrialAttributeSet>(AttributeSet);
    if (!AttributeInfo) return;
    for (auto& Pair : AS->TagsToAttributeMap)
    {
        BroadcastAttributeInfo(Pair.Key, Pair.Value());
    }
    
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
    const FGameplayAttribute& Attribute) const
{
    FRiftTrialAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(AttributeTag);
    Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
    AttributeInfoDelegate.Broadcast(Info);
}

