// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class RIFTTRIAL_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual int32 GetPlayerLevel();
	virtual int32 GetTeamID() const = 0;
	virtual FVector GetCombatSocketLocation();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	virtual void Die() = 0;

	virtual void HighlightActor(int32 StencilValue) = 0;
	virtual void UnHighlightActor() = 0;
};
