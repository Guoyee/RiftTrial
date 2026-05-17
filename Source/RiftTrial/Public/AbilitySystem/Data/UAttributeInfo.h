// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "UAttributeInfo.generated.h"

USTRUCT(Blueprintable)
struct FRiftTrialAttributeInfo
{
    GENERATED_BODY()
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag AttributeTag = FGameplayTag();
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText AttributeName = FText();
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText AttributeDescription = FText();
    
    UPROPERTY(BlueprintReadOnly)
    float AttributeValue = 0.0f;
};


/**
 * 
 */
UCLASS()
class RIFTTRIAL_API UAttributeInfo : public UDataAsset
{
    GENERATED_BODY()
    
public:
    FRiftTrialAttributeInfo FindAttributeInfoByTag(const FGameplayTag& AttributeTag, bool bLogNotFound = true) const;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FRiftTrialAttributeInfo> AttributeInfoList;
    
};
