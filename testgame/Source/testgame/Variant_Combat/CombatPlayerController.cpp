
#include "Variant_Combat/CombatPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "CombatCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "testgame.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ACombatPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(Logtestgame, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void ACombatPlayerController::SetupInputComponent()
{
	
	if (IsLocalPlayerController())
	{
		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void ACombatPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	InPawn->OnDestroyed.AddDynamic(this, &ACombatPlayerController::OnPawnDestroyed);
}

void ACombatPlayerController::SetRespawnTransform(const FTransform& NewRespawn)
{
	
	RespawnTransform = NewRespawn;
}

void ACombatPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
{
	
	if (ACombatCharacter* RespawnedCharacter = GetWorld()->SpawnActor<ACombatCharacter>(CharacterClass, RespawnTransform))
	{
		
		Possess(RespawnedCharacter);
	}
}