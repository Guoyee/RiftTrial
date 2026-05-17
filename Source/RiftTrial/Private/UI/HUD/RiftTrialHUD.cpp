// Copyright Yerik Guo


#include "UI/HUD/RiftTrialHUD.h"
#include "UI/Widgets/RiftTrialUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"


UOverlayWidgetController* ARiftTrialHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
    if (OverlayWidgetController == nullptr)
    {
        OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
        OverlayWidgetController->SetWidgetControllerParams(WCParams);
        OverlayWidgetController->BindCallbacksToDependencies();
    }
    return OverlayWidgetController;
}

UAttributeMenuWidgetController* ARiftTrialHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
    if (AttributeMenuWidgetController == nullptr)
    {
        AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
        AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
        AttributeMenuWidgetController->BindCallbacksToDependencies();
    }
    return AttributeMenuWidgetController;
}

void ARiftTrialHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
    checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialied, please fill out BP_RiftTrialHUD"));
    checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller class uninitialied, please fill out BP_RiftTrialHUD"));
    
    UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
    OverlayWidget = Cast<URiftTrialUserWidget>(Widget);
    
    const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
    UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
    
    OverlayWidget->SetWidgetController(WidgetController);
    WidgetController->BroadcastInitialValues();
    Widget->AddToViewport();
    
}
