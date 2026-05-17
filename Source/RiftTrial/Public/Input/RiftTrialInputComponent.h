// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "RiftTrialInputConfig.h"
#include "EnhancedInputComponent.h"
#include "RiftTrialInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class RIFTTRIAL_API URiftTrialInputComponent : public UEnhancedInputComponent
{
    GENERATED_BODY()
public:
    template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFunctype>
    void BindAbilityActions(const URiftTrialInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFunctype HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFunctype>
void URiftTrialInputComponent::BindAbilityActions(const URiftTrialInputConfig* InputConfig, UserClass* Object,
    PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFunctype HeldFunc)
{
    check(InputConfig);
    
    for (const FAuraInputAction& Action : InputConfig->InputActions)
    {
        if (Action.InputAction && Action.InputTag.IsValid())
        {
            if (PressedFunc)
            {
                BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
            }
            
            if (ReleasedFunc)
            {
                BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
            }
            
            if (HeldFunc)
            {
                BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
            }
        }
    }
}
