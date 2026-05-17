// Copyright Yerik Guo


#include "UI/WidgetController/RiftTrialWidgetController.h"

void URiftTrialWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
    PlayerController = WCParams.PlayerController;
    PlayerState = WCParams.PlayerState;
    AbilitySystemComponent = WCParams.AbilitySystemComponent;
    AttributeSet = WCParams.AttributeSet;
}

void URiftTrialWidgetController::BroadcastInitialValues()
{
}

void URiftTrialWidgetController::BindCallbacksToDependencies()
{
}

