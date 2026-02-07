
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatDamageable.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class UCombatDamageable : public UInterface
{
	GENERATED_BODY()
};

class ICombatDamageable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Damageable")
	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) = 0;

	UFUNCTION(BlueprintCallable, Category="Damageable")
	virtual void HandleDeath() = 0;

	UFUNCTION(BlueprintCallable, Category="Damageable")
	virtual void ApplyHealing(float Healing, AActor* Healer) = 0;
};
