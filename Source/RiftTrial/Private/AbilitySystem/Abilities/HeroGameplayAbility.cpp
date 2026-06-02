// Copyright Yerik Guo

#include "AbilitySystem/Abilities/HeroGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/RiftTrialCharacter.h"
#include "GameplayEffect.h"
#include "RiftTrialGameplayTags.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"

// ── 冷却 ──

const FGameplayTagContainer* UHeroGameplayAbility::GetCooldownTags() const
{
	CachedCooldownTags.Reset();

	// 合并 GE 自身标签 + 动态 CooldownTag
	if (const UGameplayEffect* CDO = GetCooldownGameplayEffect())
	{
		CachedCooldownTags.AppendTags(CDO->GetGrantedTags());
	}

	if (CooldownTag.IsValid())
	{
		CachedCooldownTags.AddTag(CooldownTag);
	}

	return &CachedCooldownTags;
}

void UHeroGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (!CooldownGE) return;

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC) return;

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
		CooldownGE->GetClass(), GetAbilityLevel());
	if (!SpecHandle.IsValid()) return;

	if (BaseCooldownDuration > 0.f)
	{
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			FRiftTrialGameplayTags::Get().Data_CooldownDuration, BaseCooldownDuration);
	}

	if (CooldownTag.IsValid())
	{
		FGameplayTagContainer TempContainer;
		TempContainer.AddTag(CooldownTag);
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(TempContainer);
	}

	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}

// ── 自身 Buff ──

void UHeroGameplayAbility::ApplySelfBuffs()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	for (const FSelfBuffDef& BuffDef : SelfBuffs)
	{
		if (!BuffDef.EffectClass) continue;

		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.SetAbility(this);
		Context.AddSourceObject(GetAvatarActorFromActorInfo());

		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(
			BuffDef.EffectClass, GetAbilityLevel(), Context);
		if (!Spec.IsValid()) continue;

		if (BuffDef.Duration > 0.f)
		{
			Spec.Data.Get()->SetSetByCallerMagnitude(
				FRiftTrialGameplayTags::Get().Data_BuffDuration, BuffDef.Duration);
		}

		if (BuffDef.Magnitude != 0.f)
		{
			Spec.Data.Get()->SetSetByCallerMagnitude(
				FRiftTrialGameplayTags::Get().Data_BuffMagnitude, BuffDef.Magnitude);
		}

		if (BuffDef.GrantedTag.IsValid())
		{
			FGameplayTagContainer TempContainer;
			TempContainer.AddTag(BuffDef.GrantedTag);
			Spec.Data.Get()->DynamicGrantedTags.AppendTags(TempContainer);
		}


        ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    }
}

// ── 鼠标悬停目标（指向性技能）──

AActor* UHeroGameplayAbility::GetCursorTarget() const
{
	const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo();
	if (!Info) return nullptr;

	if (const APlayerController* PC = Info->PlayerController.Get())
	{
		FHitResult Hit;
		PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
		if (Hit.bBlockingHit)
		{
			return Hit.GetActor();
		}
	}
	return nullptr;
}

// ── 蒙太奇轮播 ──

UAnimMontage* UHeroGameplayAbility::GetNextAttackMontage()
{
	if (AttackMontages.IsEmpty()) return nullptr;

	int32 Index = 0;
	bool bIndexFound = false;

	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		if (APawn* Pawn = Cast<APawn>(Avatar))
		{
			// 优先 AI 黑板
			if (AAIController* AIC = Cast<AAIController>(Pawn->GetController()))
			{
				if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
				{
					Index = BB->GetValueAsInt("ComboIndex");
					bIndexFound = true;
					const int32 NextIndex = (Index + 1) % AttackMontages.Num();
					BB->SetValueAsInt("ComboIndex", NextIndex);
				}
			}

			// 玩家英雄
			if (!bIndexFound)
			{
				if (ARiftTrialCharacter* Hero = Cast<ARiftTrialCharacter>(Pawn))
				{
					Index = Hero->IncrementAndGetComboIndex(AttackMontages.Num());
				}
			}
		}
	}

	return AttackMontages[Index].Get();
}

// ── 伤害工具（和 URiftTrialDamageGameplayAbility 共用逻辑，此处为英雄分支）──

AActor* UHeroGameplayAbility::GetAttackTarget()
{
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		if (APawn* Pawn = Cast<APawn>(Avatar))
		{
			// 优先 AI 黑板（无 AI 时走角色属性）
			if (AAIController* AIC = Cast<AAIController>(Pawn->GetController()))
			{
				if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
				{
					AActor* Target = Cast<AActor>(BB->GetValueAsObject("TargetToFollow"));
					if (IsValid(Target)) return Target;
				}
			}

			if (const ARiftTrialCharacter* Hero = Cast<ARiftTrialCharacter>(Pawn))
			{
				return Hero->GetCurrentAttackTarget();
			}
		}
	}
	return nullptr;
}

float UHeroGameplayAbility::GetAttackSpeed() const
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (const URiftTrialAttributeSet* AS = Cast<URiftTrialAttributeSet>(
			ASC->GetAttributeSet(URiftTrialAttributeSet::StaticClass())))
		{
			return FMath::Max(AS->GetAttackSpeed(), 0.1f);
		}
	}
	return 1.0f;
}

void UHeroGameplayAbility::ApplyDamageToTarget(AActor* Target)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	if (!IsValid(Target) || !DamageEffectClass) return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	UAbilitySystemComponent* TargetASC = Target->FindComponentByClass<UAbilitySystemComponent>();
	if (!TargetASC) return;

	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.SetAbility(this);
	EffectContext.AddSourceObject(GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffectClass, GetAbilityLevel(), EffectContext);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ApplyDamageToTarget: Failed to create GE spec"));
		return;
	}

	for (const auto& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
	}

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}
