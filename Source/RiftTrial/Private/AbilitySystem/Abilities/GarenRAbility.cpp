// Copyright Yerik Guo

#include "AbilitySystem/Abilities/GarenRAbility.h"
#include "AbilitySystemComponent.h"
#include "Character/RiftTrialCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Interaction/CombatInterface.h"
#include "RiftTrialGameplayTags.h"

bool UGarenRAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayTagContainer* SourceTags,
                                        const FGameplayTagContainer* TargetTags,
                                        FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// 无有效悬停目标 → 不允许激活（不打断 E、不进入冷却）
	if (!ActorInfo) return false;
	const APlayerController* PC = ActorInfo->PlayerController.Get();
	if (!PC) return false;

	AActor* Target = nullptr;

	// 优先使用缓冲目标（走近后鼠标可能已移开）
	if (ActorInfo->AvatarActor.IsValid())
	{
		if (const ARiftTrialCharacter* Hero = Cast<ARiftTrialCharacter>(ActorInfo->AvatarActor.Get()))
		{
			Target = Hero->GetBufferedAbilityTarget();
		}
	}

	// 回退到鼠标悬停目标
	if (!Target)
	{
		FHitResult Hit;
		PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
		Target = Hit.GetActor();
	}

	if (!IsValid(Target)) return false;

	// 必须实现 ICombatInterface（排除地板等）
	ICombatInterface* TargetCombat = Cast<ICombatInterface>(Target);
	if (!TargetCombat) return false;

	// 不能攻击同队
	if (const AActor* Avatar = ActorInfo->AvatarActor.Get())
	{
		if (const ICombatInterface* MyCombat = Cast<ICombatInterface>(Avatar))
		{
			const int32 MyTeam = MyCombat->GetTeamID();
			const int32 TargetTeam = TargetCombat->GetTeamID();
			if (MyTeam != 0 && TargetTeam == MyTeam) return false;
		}
	}

	// 目标不能已死亡
	const UAbilitySystemComponent* TargetASC = Target->FindComponentByClass<UAbilitySystemComponent>();
	if (TargetASC && TargetASC->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().State_Dead))
	{
		return false;
	}

	// 距离检查
	if (const AActor* Avatar = ActorInfo->AvatarActor.Get())
	{
		const float Dist = FVector::Dist(Avatar->GetActorLocation(), Target->GetActorLocation());
		if (Dist > CastRange) return false;
	}

	return true;
}

void UGarenRAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 清除缓冲目标（避免 Tick 在施法期间继续走近）
	if (ActorInfo->AvatarActor.IsValid())
	{
		if (ARiftTrialCharacter* Hero = Cast<ARiftTrialCharacter>(ActorInfo->AvatarActor.Get()))
		{
			Hero->ClearBufferedAbilityTarget();
		}
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	RTarget = GetCursorTarget();
	if (!IsValid(RTarget))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 停止移动
	if (ACharacter* Avatar = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		Avatar->GetCharacterMovement()->StopMovementImmediately();
	}

	// VFX 施加在目标身上
	if (UAbilitySystemComponent* TargetASC = RTarget->FindComponentByClass<UAbilitySystemComponent>())
	{
		TargetASC->AddGameplayCue(FRiftTrialGameplayTags::Get().GameplayCue_Garen_R);
	}

	if (RMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, RMontage, 1.0f);
		MontageTask->OnCompleted.AddDynamic(this, &UGarenRAbility::OnRMontageCompleted);
		MontageTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* WaitHitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FRiftTrialGameplayTags::Get().Event_Montage_AttackHit,
		nullptr,
		true,
		false);
	WaitHitTask->EventReceived.AddDynamic(this, &UGarenRAbility::OnRHitReceived);
	WaitHitTask->ReadyForActivation();
}

void UGarenRAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo,
                                bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(RTarget))
	{
		if (UAbilitySystemComponent* TargetASC = RTarget->FindComponentByClass<UAbilitySystemComponent>())
		{
			TargetASC->RemoveGameplayCue(FRiftTrialGameplayTags::Get().GameplayCue_Garen_R);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGarenRAbility::OnRHitReceived(FGameplayEventData Payload)
{
	ApplyDamageToTarget(RTarget.Get());
}

void UGarenRAbility::OnRMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
		GetCurrentActivationInfo(), true, false);
}
