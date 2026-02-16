
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatLifeBar.generated.h"

UCLASS(abstract)
class UCombatLifeBar : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, Category="Life Bar")
	void SetLifePercentage(float Percent);

	UFUNCTION(BlueprintImplementableEvent, Category="Life Bar")
	void SetBarColor(FLinearColor Color);
};
