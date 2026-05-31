// Copyright Yerik Guo

#include "AbilitySystem/Abilities/GarenEAbility.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "Interaction/CombatInterface.h"
#include "RiftTrialGameplayTags.h"
#include "TimerManager.h"

void UGarenEAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 激活 VFX
	ASC->AddGameplayCue(FRiftTrialGameplayTags::Get().GameplayCue_Garen_E_Spin);

	// 播放旋转蒙太奇（循环）
	if (SpinMontage)
	{
		if (AActor* Avatar = GetAvatarActorFromActorInfo())
		{
			if (USkeletalMeshComponent* Mesh = Avatar->FindComponentByClass<USkeletalMeshComponent>())
			{
				if (UAnimInstance* AnimInst = Mesh->GetAnimInstance())
				{
					AnimInst->Montage_Play(SpinMontage);
					}
			}
		}
	}

	// 启动伤害计时器
	UWorld* World = GetWorld();
	if (World)
	{
		// 第一次伤害立即触发
		DealSpinDamage();

		World->GetTimerManager().SetTimer(SpinTimerHandle, this,
			&UGarenEAbility::OnSpinTick, TickInterval, true, TickInterval);

		// 总持续时间结束后自动关闭
		World->GetTimerManager().SetTimer(DurationTimerHandle, this,
			&UGarenEAbility::K2_EndAbility, SpinDuration, false);
	}

}

void UGarenEAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo,
                                bool bReplicateEndAbility, bool bWasCancelled)
{
	// 移除 VFX
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
				ASC->RemoveGameplayCue(FRiftTrialGameplayTags::Get().GameplayCue_Garen_E_Spin);
	}

	// 停止蒙太奇并解绑
	if (SpinMontage)
	{
		if (AActor* Avatar = GetAvatarActorFromActorInfo())
		{
			if (USkeletalMeshComponent* Mesh = Avatar->FindComponentByClass<USkeletalMeshComponent>())
			{
				if (UAnimInstance* AnimInst = Mesh->GetAnimInstance())
				{
					AnimInst->Montage_Stop(0.2f, SpinMontage);
				}
			}
		}
	}

	// 清理计时器
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SpinTimerHandle);
		World->GetTimerManager().ClearTimer(DurationTimerHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGarenEAbility::OnSpinTick()
{
	DealSpinDamage();
}

void UGarenEAbility::DealSpinDamage()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !DamageEffectClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector Center = Avatar->GetActorLocation();

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Avatar);

	World->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(SpinRadius),
		QueryParams);

	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* HitActor = Result.GetActor();
		if (!IsValid(HitActor)) continue;

		// 不伤害自己 / 同队目标 / 死目标
		if (HitActor == Avatar) continue;

		// 团队检查
		if (ICombatInterface* MyCombat = Cast<ICombatInterface>(Avatar))
		{
			if (ICombatInterface* HitCombat = Cast<ICombatInterface>(HitActor))
			{
				const int32 MyTeam = MyCombat->GetTeamID();
				const int32 HitTeam = HitCombat->GetTeamID();
				if (MyTeam != 0 && HitTeam == MyTeam) continue;
			}
		}

		if (UAbilitySystemComponent* HitASC = HitActor->FindComponentByClass<UAbilitySystemComponent>())
		{
			if (HitASC->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().State_Dead))
				continue;
		}

		ApplyDamageToTarget(HitActor);
	}
}
