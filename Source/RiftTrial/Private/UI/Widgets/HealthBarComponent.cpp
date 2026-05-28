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

	// Widget 可能在 BindToAbilitySystem 之后才创建，补推当前值
	DetermineTeamColor();
	UpdateDisplay();
	UpdateManaDisplay();
	if (CachedWidget)
	{
		CachedWidget->OnLevelUpdated(PlayerLevel);
	}
}

void UHealthBarComponent::BindToAbilitySystem(UAbilitySystemComponent* InASC)
{
	if (!InASC) return;

	const URiftTrialAttributeSet* AS = InASC->GetSet<URiftTrialAttributeSet>();
	if (!AS) return;

	CurrentHealth = AS->GetHealth();
	CurrentMaxHealth = AS->GetMaxHealth();
	CurrentMana = AS->GetMana();
	CurrentMaxMana = AS->GetMaxMana();

	InASC->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute())
	     .AddUObject(this, &UHealthBarComponent::OnHealthChanged);

	InASC->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute())
	     .AddUObject(this, &UHealthBarComponent::OnMaxHealthChanged);

	InASC->GetGameplayAttributeValueChangeDelegate(AS->GetManaAttribute())
	     .AddUObject(this, &UHealthBarComponent::OnManaChanged);

	InASC->GetGameplayAttributeValueChangeDelegate(AS->GetMaxManaAttribute())
	     .AddUObject(this, &UHealthBarComponent::OnMaxManaChanged);

	DetermineTeamColor();

	UpdateDisplay();
	UpdateManaDisplay();
}

void UHealthBarComponent::DetermineTeamColor()
{
	if (!CachedWidget)
	{
		CachedWidget = Cast<UHealthBarWidget>(GetUserWidgetObject());
	}
	if (!CachedWidget) return;

	bool bIsEnemy = false;
	bool bIsSelf = false;
	if (const ICombatInterface* OwnerCombat = Cast<ICombatInterface>(GetOwner()))
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (APawn* PlayerPawn = PC->GetPawn())
			{
				bIsSelf = (GetOwner() == PlayerPawn);
				if (!bIsSelf)
				{
					if (const ICombatInterface* PlayerCombat = Cast<ICombatInterface>(PlayerPawn))
					{
						const int32 OwnerTeam = OwnerCombat->GetTeamID();
						bIsEnemy = (OwnerTeam != 0 && OwnerTeam != PlayerCombat->GetTeamID());
					}
				}
			}
		}
	}

	CachedWidget->SetTeamColor(bIsEnemy, bIsSelf);
}

void UHealthBarComponent::SetPlayerLevel(int32 Level)
{
	PlayerLevel = Level;
	if (CachedWidget)
	{
		CachedWidget->OnLevelUpdated(PlayerLevel);
	}
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

void UHealthBarComponent::OnManaChanged(const FOnAttributeChangeData& Data)
{
	CurrentMana = Data.NewValue;
	UpdateManaDisplay();
}

void UHealthBarComponent::OnMaxManaChanged(const FOnAttributeChangeData& Data)
{
	CurrentMaxMana = Data.NewValue;
	UpdateManaDisplay();
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

void UHealthBarComponent::UpdateManaDisplay()
{
	if (!CachedWidget)
	{
		CachedWidget = Cast<UHealthBarWidget>(GetUserWidgetObject());
	}

	if (CachedWidget)
	{
		CachedWidget->OnManaUpdated(CurrentMana, CurrentMaxMana);
	}
}
