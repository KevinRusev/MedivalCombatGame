
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatAttacker.h"
#include "CombatDamageable.h"
#include "Animation/AnimMontage.h"
#include "Engine/TimerHandle.h"
#include "CombatEnemy.generated.h"

class UWidgetComponent;
class UCombatLifeBar;
class UAnimMontage;

DECLARE_DELEGATE(FOnEnemyAttackCompleted);

DECLARE_DELEGATE(FOnEnemyLanded);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDied);

UCLASS(abstract)
class ACombatEnemy : public ACharacter, public ICombatAttacker, public ICombatDamageable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* LifeBar;

public:
	
	ACombatEnemy();

protected:

	UPROPERTY(EditAnywhere, Category="Damage")
	float MaxHP = 3.0f;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Damage", meta = (ClampMin = 0, ClampMax = 100))
	float CurrentHP = 0.0f;

protected:

	UPROPERTY(EditAnywhere, Category="Damage")
	FName PelvisBoneName;

	UPROPERTY(EditAnywhere, Category="Damage")
	UCombatLifeBar* LifeBarWidget;

	bool bIsAttacking = false;

	UPROPERTY(EditAnywhere, Category="Melee Attack|Trace", meta = (ClampMin = 0, ClampMax = 500, Units = "cm"))
	float MeleeTraceDistance = 75.0f;

	UPROPERTY(EditAnywhere, Category="Melee Attack|Trace", meta = (ClampMin = 0, ClampMax = 500, Units = "cm"))
	float MeleeTraceRadius = 50.0f;

	UPROPERTY(EditAnywhere, Category="Melee Attack|Damage", meta = (ClampMin = 0, ClampMax = 100))
	float MeleeDamage = 1.0f;

	UPROPERTY(EditAnywhere, Category="Melee Attack|Damage", meta = (ClampMin = 0, ClampMax = 1000, Units = "cm/s"))
	float MeleeKnockbackImpulse = 150.0f;

	UPROPERTY(EditAnywhere, Category="Melee Attack|Damage", meta = (ClampMin = 0, ClampMax = 1000, Units = "cm/s"))
	float MeleeLaunchImpulse = 350.0f;

	UPROPERTY(EditAnywhere, Category="Melee Attack|Combo")
	UAnimMontage* ComboAttackMontage;

	UPROPERTY(EditAnywhere, Category="Melee Attack|Combo")
	TArray<FName> ComboSectionNames;

	int32 TargetComboCount = 0;

	int32 CurrentComboAttack = 0;

	UPROPERTY(EditAnywhere, Category="Melee Attack|Charged")
	UAnimMontage* ChargedAttackMontage;

	UPROPERTY(EditAnywhere, Category="Melee Attack|Charged")
	FName ChargeLoopSection;

	UPROPERTY(EditAnywhere, Category="Melee Attack|Charged")
	FName ChargeAttackSection;

	UPROPERTY(EditAnywhere, Category="Melee Attack|Charged", meta = (ClampMin = 1, ClampMax = 20))
	int32 MinChargeLoops = 2;

	UPROPERTY(EditAnywhere, Category="Melee Attack|Charged", meta = (ClampMin = 1, ClampMax = 20))
	int32 MaxChargeLoops = 5;

	int32 TargetChargeLoops = 0;

	int32 CurrentChargeLoop = 0;

	UPROPERTY(EditAnywhere, Category="Death")
	float DeathRemovalTime = 5.0f;

	FTimerHandle DeathTimer;

	FOnMontageEnded OnAttackMontageEnded;

public:
	
	FOnEnemyAttackCompleted OnAttackCompleted;

	FOnEnemyLanded OnEnemyLanded;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnEnemyDied OnEnemyDied;

public:

	void DoAIComboAttack();

	void DoAIChargedAttack();

	void AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:

	virtual void DoAttackTrace(FName DamageSourceBone) override;

	UFUNCTION(BlueprintCallable, Category="Attacker")
	virtual void CheckCombo() override;

	UFUNCTION(BlueprintCallable, Category="Attacker")
	virtual void CheckChargedAttack() override;

	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;

	virtual void HandleDeath() override;

	virtual void ApplyHealing(float Healing, AActor* Healer) override;

protected:

	void RemoveFromLevel();

public:

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Landed(const FHitResult& Hit) override;

protected:

	UFUNCTION(BlueprintImplementableEvent, Category="Combat")
	void ReceivedDamage(float Damage, const FVector& ImpactPoint, const FVector& DamageDirection);

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};
