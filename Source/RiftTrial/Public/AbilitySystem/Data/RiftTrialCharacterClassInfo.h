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

    // 英雄基础属性 GE（所有基础属性在此 GE 中设定）
    UPROPERTY(EditDefaultsOnly, Category = CAT_ATTRIBUTE)
    TSubclassOf<UGameplayEffect> DefaultAttributes;
};

UCLASS()
class RIFTTRIAL_API URiftTrialCharacterClassInfo : public UDataAsset
{
    GENERATED_BODY()
public:
    // 英雄职业 → 默认属性 GE
    UPROPERTY(EditDefaultsOnly, Category = CAT_ATTRIBUTE)
    TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

    // 小兵等非英雄单位共用属性 GE
    UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
    TSubclassOf<UGameplayEffect> MinionDefaultAttributes;

    // 所有单位共有的 Ability
    UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
    TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

    FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
};
