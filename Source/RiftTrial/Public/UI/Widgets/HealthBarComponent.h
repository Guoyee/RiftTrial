// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

class UAbilitySystemComponent;
class UHealthBarWidget;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RIFTTRIAL_API UHealthBarComponent : public UWidgetComponent
{
    GENERATED_BODY()

public:
    UHealthBarComponent();

    UFUNCTION(BlueprintCallable)
    void BindToAbilitySystem(UAbilitySystemComponent* InASC);

    UFUNCTION(BlueprintCallable)
    void SetPlayerLevel(int32 Level);

    UFUNCTION(BlueprintPure)
    float GetHealthPercent() const { return HealthPercent; }

    virtual void InitWidget() override;

protected:
    void OnHealthChanged(const struct FOnAttributeChangeData& Data);
    void OnMaxHealthChanged(const struct FOnAttributeChangeData& Data);
    void OnManaChanged(const struct FOnAttributeChangeData& Data);
    void OnMaxManaChanged(const struct FOnAttributeChangeData& Data);
    void UpdateDisplay();
    void UpdateManaDisplay();

    UPROPERTY(BlueprintReadOnly, Category = "RiftTrial|UI")
    float HealthPercent = 1.f;

private:
    void DetermineTeamColor();

    float CurrentHealth = 1.f;
    float CurrentMaxHealth = 1.f;
    float CurrentMana = 0.f;
    float CurrentMaxMana = 0.f;
    int32 PlayerLevel = 1;

    UPROPERTY()
    TObjectPtr<UHealthBarWidget> CachedWidget;
};
