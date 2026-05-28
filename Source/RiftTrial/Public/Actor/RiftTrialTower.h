// Copyright Yerik Guo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interaction/CombatInterface.h"
#include "RiftTrial.h"
#include "RiftTrialTower.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayAbility;
class USphereComponent;
class USkeletalMeshComponent;
class UHealthBarComponent;
class UCapsuleComponent;

// 防御塔：碰撞事件维护目标列表，按优先级自动攻击
UCLASS(PrioritizeCategories = "RiftTrial")
class RIFTTRIAL_API ARiftTrialTower : public AActor, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	ARiftTrialTower();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// ICombatInterface
	virtual int32 GetTeamID() const override;
	virtual FVector GetCombatSocketLocation() override;
	virtual void Die() override;
	virtual void HighlightActor(int32 StencilValue) override;
	virtual void UnHighlightActor() override;

	UFUNCTION(BlueprintCallable, Category = "RiftTrial|Ability")
	AActor* SelectTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RiftTrial|Team")
	int32 TeamID = 0;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RiftTrial|UI")
	TObjectPtr<UHealthBarComponent> HealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> TowerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> DetectionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> CursorCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = "RiftTrial|Combat")
	FName ProjectileSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "RiftTrial|Attribute")
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	// 身份标签 GE（Type.Tower），Duration=Infinite
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "RiftTrial|Attribute")
	TSubclassOf<UGameplayEffect> IdentityEffect;

	UPROPERTY(EditAnywhere, Category = "RiftTrial|Ability")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	void InitializeAttributes() const;
	void AddAbilities() const;

private:
	UPROPERTY()
	TArray<AActor*> OverlappingActors;

	bool bIsDead = false;
};
