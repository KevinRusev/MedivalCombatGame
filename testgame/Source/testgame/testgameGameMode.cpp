
#include "testgameGameMode.h"
#include "testgameGameplayHUD.h"
#include "NavigationSystem.h"
#include "Engine/Engine.h"

AtestgameGameMode::AtestgameGameMode()
{
	
	HUDClass = AtestgameGameplayHUD::StaticClass();
}

void AtestgameGameMode::BeginPlay()
{
	Super::BeginPlay();

	// BP_Enemy / anim BPs with "Print String" are not in source control — turn off all on-screen Kismet
	// prints in non-shipping builds (stops the cyan walk-speed column, etc.).
#if !UE_BUILD_SHIPPING
	if (GEngine && GetWorld())
	{
		GEngine->Exec(GetWorld(), TEXT("DisableAllScreenMessages"));
	}
#endif

	// Marketplace maps often ship without baked Recast data; PIE then has no nav and MoveToActor fails
	// ("NAVMESH NEEDS TO BE REBUILT", missing CrowdManager/Recast). Defer one tick so NavSystem is ready.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			if (UWorld* W = GetWorld())
			{
				if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(W))
				{
					NavSys->ReleaseInitialBuildingLock();
					NavSys->Build();
				}
			}
		}));
	}
}
