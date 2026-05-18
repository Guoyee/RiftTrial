// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "RiftTrialInputConfig.h"
#include "EnhancedInputComponent.h"
#include "RiftTrialInputComponent.generated.h"

UCLASS()
class RIFTTRIAL_API URiftTrialInputComponent : public UEnhancedInputComponent
{
    GENERATED_BODY()
public:
    template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFunctype>
    static void BindAbilityActions(const URiftTrialInputConfig* InputConfig, UEnhancedInputComponent* InputComp,
        UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFunctype HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFunctype>
void URiftTrialInputComponent::BindAbilityActions(const URiftTrialInputConfig* InputConfig, UEnhancedInputComponent* InputComp,
    UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFunctype HeldFunc)
{
    if (!InputConfig)
    {
        UE_LOG(LogTemp, Error, TEXT("BindAbilityActions: InputConfig is null!"));
        return;
    }
    if (!InputComp)
    {
        UE_LOG(LogTemp, Error, TEXT("BindAbilityActions: InputComp is null!"));
        return;
    }

    for (const FAuraInputAction& Action : InputConfig->InputActions)
    {
        if (Action.InputAction && Action.InputTag.IsValid())
        {
            if (PressedFunc)
            {
                InputComp->BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
            }
            if (ReleasedFunc)
            {
                InputComp->BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
            }
            if (HeldFunc)
            {
                InputComp->BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
            }
        }
    }
}
