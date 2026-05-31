#pragma once

#include "GameplayEffectTypes.h"
#include "RiftTrialAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FRiftTrialGameplayEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY();
    
public:
    
    bool IsCriticalHit() const { return bIsCriticalHit; }
    bool IsBlockedHit() const { return bIsBlockedHit; }
    
    void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
    void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
    
    /** Returns the actual struct used for serialization, subclasses must override this! */
    virtual UScriptStruct* GetScriptStruct() const
    {
        return StaticStruct();
    }
    
    virtual FGameplayEffectContext* Duplicate() const
    {
        FRiftTrialGameplayEffectContext* NewContext = new FRiftTrialGameplayEffectContext();
        //拷贝当前的内容到新创建的对象中
        *NewContext = *this;
        if (GetHitResult())
        {
            //因为HitResult是SharedPtr, 默认的=操作符只会把引用计数+1，指向的还是同一个对象
            //Dose a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
    
    
    /** Custom serialization, subclasses must override this */
    virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
    
    
protected:
    
    UPROPERTY()
    bool bIsBlockedHit = false;
    
    UPROPERTY()
    bool bIsCriticalHit = false;
    
    
};

template<>
struct TStructOpsTypeTraits< FRiftTrialGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FRiftTrialGameplayEffectContext >
{
    enum
    {
        WithNetSerializer = true,
        WithCopy = true        // Necessary so that TSharedPtr<FHitResult> Data is copied around
    };
};
   
