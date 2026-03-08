
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlatformingPlayerController.generated.h"

class UInputMappingContext;
class APlatformingCharacter;

UCLASS(abstract)
class APlatformingPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	TObjectPtr<UUserWidget> MobileControlsWidget;

	UPROPERTY(EditAnywhere, Category="Respawn")
	TSubclassOf<APlatformingCharacter> CharacterClass;

protected:

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedActor);
};
