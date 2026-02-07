
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatAttacker.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class UCombatAttacker : public UInterface
{
	GENERATED_BODY()
};

class ICombatAttacker
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Attacker")
	virtual void DoAttackTrace(FName DamageSourceBone) = 0;

	UFUNCTION(BlueprintCallable, Category="Attacker")
	virtual void CheckCombo() = 0;

	UFUNCTION(BlueprintCallable, Category="Attacker")
	virtual void CheckChargedAttack() = 0;
};
