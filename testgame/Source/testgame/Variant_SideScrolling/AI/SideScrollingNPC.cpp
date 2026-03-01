
#include "SideScrollingNPC.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

ASideScrollingNPC::ASideScrollingNPC()
{
 	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
}

void ASideScrollingNPC::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(DeactivationTimer);
}

void ASideScrollingNPC::Interaction(AActor* Interactor)
{
	
	if (bDeactivated)
	{
		return;
	}

	bDeactivated = true;

	GetCharacterMovement()->StopMovementImmediately();

	FVector LaunchVector = Interactor->GetActorForwardVector() * LaunchImpulse;
	LaunchVector.Y = 0.0f;
	LaunchVector.Z = LaunchVerticalImpulse;

	LaunchCharacter(LaunchVector, true, true);

	GetWorld()->GetTimerManager().SetTimer(DeactivationTimer, this, &ASideScrollingNPC::ResetDeactivation, DeactivationTime, false);
}

void ASideScrollingNPC::ResetDeactivation()
{
	
	bDeactivated = false;
}
