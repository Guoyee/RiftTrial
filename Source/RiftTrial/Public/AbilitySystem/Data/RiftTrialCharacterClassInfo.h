// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RiftTrialCharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
    None,
    Elementalist,
    Warrior,
    Ranger
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
    GENERATED_BODY()
    
    UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
    TSubclassOf<UGameplayEffect> PrimaryAttributes;
    
};

/**
 * 
 */
UCLASS()
class RIFTTRIAL_API URiftTrialCharacterClassInfo : public UDataAsset
{
    GENERATED_BODY()
public:
    
    UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
    TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;
    
    UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
    TSubclassOf<UGameplayEffect> SecondaryAttributes;
    
    UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
    TSubclassOf<UGameplayEffect> VitalAttributes;
    
    //添加所有Enemy共有的Ability
    UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
    TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;
    
    FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass characterClass);
};
