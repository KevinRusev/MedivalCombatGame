
#include "SideScrollingPickup.h"
#include "GameFramework/Character.h"
#include "SideScrollingGameMode.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"

ASideScrollingPickup::ASideScrollingPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Sphere->SetupAttachment(RootComponent);

	Sphere->SetSphereRadius(100.0f);

	Sphere->SetCollisionObjectType(ECC_WorldDynamic);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	OnActorBeginOverlap.AddDynamic(this, &ASideScrollingPickup::BeginOverlap);
}

void ASideScrollingPickup::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	
	if (ACharacter* OverlappedCharacter = Cast<ACharacter>(OtherActor))
	{
		
		if (OverlappedCharacter->IsPlayerControlled())
		{
			
			if (ASideScrollingGameMode* GM = Cast<ASideScrollingGameMode>(GetWorld()->GetAuthGameMode()))
			{
				
				GM->ProcessPickup();

				SetActorEnableCollision(false);

				BP_OnPickedUp();
			}
		}
	}
}