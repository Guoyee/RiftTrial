// Copyright Yerik Guo

#include "UI/Widgets/HealthBarComponent.h"
#include "UI/Widgets/HealthBarWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"
#include "Interaction/CombatInterface.h"

UHealthBarComponent::UHealthBarComponent()
{
	SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	SetWidgetSpace(EWidgetSpace::Screen);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetDrawAtDesiredSize(true);
}

void UHealthBarComponent::InitWidget()
{
	Super::InitWidget();

	if (UUserWidget* WidgetObj = GetUserWidgetObject())
	{
		CachedWidget = Cast<UHealthBarWidget>(WidgetObj);
	}
}

void UHealthBarComponent::BindToAbilitySystem(UAbilitySystemComponent* InASC)
{
	if (!InASC) return;

	const URiftTrialAttributeSet* AS = InASC->GetSet<URiftTrialAttributeSet>();
	if (!AS) return;

	CurrentHealth = AS->GetHealth();
	CurrentMaxHealth = AS->GetMaxHealth();

	InASC->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute())
	     .AddUObject(this, &UHealthBarComponent::OnHealthChanged);

	InASC->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute())
	     .AddUObject(this, &UHealthBarComponent::OnMaxHealthChanged);

	// 队伍颜色只设一次
	DetermineTeamColor();

	UpdateDisplay();
}

void UHealthBarComponent::DetermineTeamColor()
{
	if (!CachedWidget)
	{
		CachedWidget = Cast<UHealthBarWidget>(GetUserWidgetObject());
	}
	if (!CachedWidget) return;

	bool bIsEnemy = false;
	if (const ICombatInterface* OwnerCombat = Cast<ICombatInterface>(GetOwner()))
	{
		const int32 OwnerTeam = OwnerCombat->GetTeamID();
		if (OwnerTeam != 0)
		{
			if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
			{
				if (const ICombatInterface* PlayerCombat = Cast<ICombatInterface>(PC->GetPawn()))
				{
					bIsEnemy = (OwnerTeam != PlayerCombat->GetTeamID());
				}
			}
		}
	}

	CachedWidget->SetTeamColor(bIsEnemy);
}

void UHealthBarComponent::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentHealth = Data.NewValue;
	UpdateDisplay();
}

void UHealthBarComponent::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentMaxHealth = Data.NewValue;
	UpdateDisplay();
}

void UHealthBarComponent::UpdateDisplay()
{
	HealthPercent = (CurrentMaxHealth > 0.f) ? FMath::Clamp(CurrentHealth / CurrentMaxHealth, 0.f, 1.f) : 1.f;

	const bool bShouldShow = (HealthPercent > 0.f);
	SetVisibility(bShouldShow);

	if (!CachedWidget)
	{
		CachedWidget = Cast<UHealthBarWidget>(GetUserWidgetObject());
	}

	if (CachedWidget)
	{
		CachedWidget->OnHealthPercentUpdated(HealthPercent);
		CachedWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}
