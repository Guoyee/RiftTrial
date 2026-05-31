// Copyright Yerik Guo

#include "AbilitySystem/Abilities/HeroMeleeAttackAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "RiftTrialGameplayTags.h"

void UHeroMeleeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Q 强化普攻 → 使用专属蒙太奇；否则走连击轮播
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	
	const bool bIsQEmpowered = ASC && ASC->HasMatchingGameplayTag(
		FRiftTrialGameplayTags::Get().State_EmpoweredAttack);

	UE_LOG(LogTemp, Warning, TEXT("[HeroAttack] bIsQEmpowered=%d, ASC=%s"),
		bIsQEmpowered, ASC ? *ASC->GetName() : TEXT("null"));

	UAnimMontage* Montage = bIsQEmpowered && QAttackMontage
		? QAttackMontage.Get()
		: GetNextAttackMontage();

	if (!Montage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const float PlayRate = GetAttackSpeed();

	// 播放蒙太奇（BlendOut 设为 0 可在蒙太奇资产中配置，也可通过 StopWhenAbilityEnds 控制）
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, Montage, PlayRate);
	MontageTask->OnCompleted.AddDynamic(this, &UHeroMeleeAttackAbility::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UHeroMeleeAttackAbility::OnMontageInterrupted);
	MontageTask->ReadyForActivation();

	// 等待命中帧（Event.Montage.AttackHit）
	UAbilityTask_WaitGameplayEvent* WaitHitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FRiftTrialGameplayTags::Get().Event_Montage_AttackHit,
		nullptr,
		true,   // OnlyTriggerOnce
		false); // OnlyMatchExact
	WaitHitTask->EventReceived.AddDynamic(this, &UHeroMeleeAttackAbility::OnAttackHitReceived);
	WaitHitTask->ReadyForActivation();
}

void UHeroMeleeAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroMeleeAttackAbility::OnAttackHitReceived(FGameplayEventData Payload)
{
	AActor* Target = GetAttackTarget();
	ApplyDamageToTarget(Target);

	// 普攻命中 VFX
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->ExecuteGameplayCue(FRiftTrialGameplayTags::Get().GameplayCue_Garen_Attack);
	}

	// Q 强化普攻检测：额外伤害 + 沉默 + 消耗 Buff
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && ASC->HasMatchingGameplayTag(FRiftTrialGameplayTags::Get().State_EmpoweredAttack))
	{
		ApplyQEmpoweredAttack(Target);
		ASC->RemoveLooseGameplayTag(FRiftTrialGameplayTags::Get().State_EmpoweredAttack);
	}

	// 在命中帧提交冷却（而非蒙太奇结束）
	CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
}

void UHeroMeleeAttackAbility::ApplyQEmpoweredAttack(AActor* Target)
{
	if (!IsValid(Target)) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	// 施加 Q 额外伤害
	if (QBonusDamageEffectClass)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.SetAbility(this);
		Context.AddSourceObject(GetAvatarActorFromActorInfo());

		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(QBonusDamageEffectClass, GetAbilityLevel(), Context);
		if (Spec.IsValid())
		{
			if (UAbilitySystemComponent* TargetASC = Target->FindComponentByClass<UAbilitySystemComponent>())
			{
				ASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
			}
		}
	}

	// 施加沉默
	if (QSilenceEffectClass)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.SetAbility(this);
		Context.AddSourceObject(GetAvatarActorFromActorInfo());

		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(QSilenceEffectClass, GetAbilityLevel(), Context);
		if (Spec.IsValid())
		{
			if (UAbilitySystemComponent* TargetASC = Target->FindComponentByClass<UAbilitySystemComponent>())
			{
				ASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
			}
		}
	}
}

void UHeroMeleeAttackAbility::OnMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UHeroMeleeAttackAbility::OnMontageInterrupted()
{
	// 蒙太奇被打断（移动/新指令取消前摇）
	// 不调用 CommitAbility，冷却未触发，可立即重新激活
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}
