// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RiftTrialHUD.generated.h"

class UAttributeMenuWidgetController;
class UAbilitySystemComponent;
class UAttributeSet;
struct FWidgetControllerParams;
class UOverlayWidgetController;
class URiftTrialUserWidget;
/**
 * 
 */
UCLASS()
class RIFTTRIAL_API ARiftTrialHUD : public AHUD
{
    GENERATED_BODY()
    
public:
    
    
    UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
    UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);
    
    void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
    
    
protected:

    
private:
    UPROPERTY()
    TObjectPtr<URiftTrialUserWidget> OverlayWidget;
    
    UPROPERTY(EditAnywhere)
    TSubclassOf<URiftTrialUserWidget> OverlayWidgetClass;    
    
    UPROPERTY()
    TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
    
    UPROPERTY(EditAnywhere)
    TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
    
    
    UPROPERTY()
    TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;
    
    UPROPERTY(EditAnywhere)
    TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
};
