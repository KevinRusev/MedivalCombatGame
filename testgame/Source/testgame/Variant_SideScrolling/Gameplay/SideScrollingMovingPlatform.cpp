
#include "SideScrollingMovingPlatform.h"
#include "Components/SceneComponent.h"

ASideScrollingMovingPlatform::ASideScrollingMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ASideScrollingMovingPlatform::Interaction(AActor* Interactor)
{
	
	if (bMoving)
	{
		return;
	}

	bMoving = true;

	BP_MoveToTarget();
}

void ASideScrollingMovingPlatform::ResetInteraction()
{
	
	if (bOneShot)
	{
		return;
	}

	bMoving = false;
}
