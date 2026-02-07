
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatActivatable.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class UCombatActivatable : public UInterface
{
	GENERATED_BODY()
};

class ICombatActivatable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Activatable")
	virtual void ToggleInteraction(AActor* ActivationInstigator) = 0;

	UFUNCTION(BlueprintCallable, Category="Activatable")
	virtual void ActivateInteraction(AActor* ActivationInstigator) = 0;

	UFUNCTION(BlueprintCallable, Category="Activatable")
	virtual void DeactivateInteraction(AActor* ActivationInstigator) = 0;
};
