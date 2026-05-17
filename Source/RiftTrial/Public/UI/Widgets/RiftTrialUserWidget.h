// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RiftTrialUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class RIFTTRIAL_API URiftTrialUserWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable)
    void SetWidgetController(UObject* InWidgetController);
    
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UObject> WidgetController;
    
protected:
    UFUNCTION(BlueprintImplementableEvent)
    void WidgetControllerSet();
};
