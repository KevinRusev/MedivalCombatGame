
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UPawnSensingComponent;
class UHealthComponent;
class UAnimMontage;
class UAnimInstance;
class UAnimSequenceBase;

UCLASS(Blueprintable)
class TESTGAME_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

	UFUNCTION()
	void OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DealDamage();

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetActor() const { return TargetActor; }

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTargetActor(AActor* NewTarget) { TargetActor = NewTarget; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool CanAttack() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsTargetInRange() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsAttacking() const { return bIsAttacking; }

protected:
	
	UFUNCTION()
	void HandleDeath(UHealthComponent* HealthComp, AActor* Killer);

	UFUNCTION()
	void HandleHealthChanged(UHealthComponent* HealthComp, float NewHealth, float Delta, AActor* DamageInstigator);

	void DestroyAfterDeath();

	void FinishAttack();

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Anim")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Anim")
	UAnimSequenceBase* HitReactAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Anim")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Anim")
	UAnimSequenceBase* AttackAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (ClampMin = "0", Units = "s"))
	float AttackDamageTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Anim")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Anim")
	UAnimSequenceBase* DeathAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Anim")
	bool bForceDefaultAnimBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Anim")
	TSubclassOf<UAnimInstance> DefaultAnimInstanceClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (ClampMin = "0", Units = "s"))
	float HitStaggerDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (ClampMin = "0", Units = "s"))
	float DeathLifespan;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ChaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float PatrolSpeed;

	float LastAttackTime;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;

	UPROPERTY(Transient)
	bool bAttackUsedSingleNode;

	FTimerHandle AttackDealDamageTimer;
	FTimerHandle AttackFinishTimer;
	FTimerHandle HitStaggerTimer;
	FTimerHandle DeathFreezeTimer;
	FTimerHandle RestoreAnimBPTimer;

	UPROPERTY(Transient)
	TSubclassOf<UAnimInstance> CachedAnimInstanceClass;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsHitReacting;

	void FinishHitStagger();

	void PlayHitReactAnim();

	bool PlayDeathAnim();

	void FreezeDeathPose();

	void RestoreAnimBPIfNeeded();

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	bool bIsDead;

public:
	
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsHitReacting() const { return bIsHitReacting; }
};
