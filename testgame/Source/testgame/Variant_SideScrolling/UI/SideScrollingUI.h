
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SideScrollingUI.generated.h"

UCLASS(abstract)
class USideScrollingUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void UpdatePickups(int32 Amount);
};
