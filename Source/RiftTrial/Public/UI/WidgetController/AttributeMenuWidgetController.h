// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/RiftTrialWidgetController.h"
#include "AbilitySystem/Data/UAttributeInfo.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayTag;
struct FGameplayAttribute;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FRiftTrialAttributeInfo&, Info);

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class RIFTTRIAL_API UAttributeMenuWidgetController : public URiftTrialWidgetController
{
    GENERATED_BODY()
public:
    virtual void BroadcastInitialValues() override;
    virtual void BindCallbacksToDependencies() override;

    UPROPERTY(BlueprintAssignable, Category="GAS|Attribute")
    FAttributeInfoSignature AttributeInfoDelegate;

protected:

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UAttributeInfo> AttributeInfo;

private:
    void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
};
