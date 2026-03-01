
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SideScrollingInteractable.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class USideScrollingInteractable : public UInterface
{
	GENERATED_BODY()
};

class ISideScrollingInteractable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Interactable")
	virtual void Interaction(AActor* Interactor) = 0;

};
