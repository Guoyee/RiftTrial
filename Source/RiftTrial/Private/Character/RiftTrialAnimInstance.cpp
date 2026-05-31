// Copyright Yerik Guo

#include "Character/RiftTrialAnimInstance.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "RiftTrialGameplayTags.h"

void URiftTrialAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedASC)
	{
		if (AActor* Owner = GetOwningActor())
		{
			CachedASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
		}
	}

	if (CachedASC)
	{
		bIsQEmpowered = CachedASC->HasMatchingGameplayTag(
			FRiftTrialGameplayTags::Get().State_EmpoweredAttack);
	}
	else
	{
		bIsQEmpowered = false;
	}
}
