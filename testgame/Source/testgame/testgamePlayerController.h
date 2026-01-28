
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "testgamePlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

UCLASS(abstract)
class AtestgamePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	TObjectPtr<UUserWidget> MobileControlsWidget;

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

};
