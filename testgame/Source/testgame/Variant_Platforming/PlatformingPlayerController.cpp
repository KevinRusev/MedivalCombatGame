
#include "Variant_Platforming/PlatformingPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PlatformingCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "testgame.h"
#include "Widgets/Input/SVirtualJoystick.h"

void APlatformingPlayerController::BeginPlay()
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

void APlatformingPlayerController::SetupInputComponent()
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

void APlatformingPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	InPawn->OnDestroyed.AddDynamic(this, &APlatformingPlayerController::OnPawnDestroyed);
}

void APlatformingPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
{
	
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), ActorList);

	if (ActorList.Num() > 0)
	{
		
		const FTransform SpawnTransform = ActorList[0]->GetActorTransform();

		if (APlatformingCharacter* RespawnedCharacter = GetWorld()->SpawnActor<APlatformingCharacter>(CharacterClass, SpawnTransform))
		{
			
			Possess(RespawnedCharacter);
		}
	}
}