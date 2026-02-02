
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, UHealthComponent*, HealthComp, float, NewHealth, float, Delta, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeath, UHealthComponent*, HealthComp, AActor*, Killer);

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class TESTGAME_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

protected:
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	float ReceiveDamage(float DamageAmount, AActor* DamageInstigator);

	UFUNCTION(BlueprintCallable, Category = "Health")
	float Heal(float HealAmount, AActor* Healer);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Kill(AActor* Killer);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ResetHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const { return MaxHealth > 0.f ? FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f) : 0.f; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsInvulnerable() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetInvulnerableFor(float Duration);

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeath OnDeath;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "1.0"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0.0", Units = "s"))
	float InvulnerabilityDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

private:
	bool bIsDead;
	float LastDamagedTime;

	float InvulnerableUntilTime;
};
