// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

UCLASS()
class RIFTTRIAL_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthPercentUpdated(float NewHealthPercent);

	UFUNCTION(BlueprintImplementableEvent)
	void SetTeamColor(bool bIsEnemy);
};
